#!/usr/bin/env python3
# -*- coding: utf-8 -*-

from __future__ import annotations

import argparse
import os
import re
import shutil
from dataclasses import dataclass
from pathlib import Path
from typing import Dict, List, Optional, Tuple

IMAGE_EXTS = {".png", ".jpg", ".jpeg", ".gif", ".webp", ".bmp", ".tiff", ".tif", ".svg"}

# Obsidian embed: ![[file.png]] or ![[path/file.png|300]] etc.
OBSIDIAN_EMBED_RE = re.compile(r"!\[\[([^\]]+?)\]\]")

# Markdown image: ![alt](path)
MARKDOWN_IMAGE_RE = re.compile(r"!\[([^\]]*)\]\(([^)]+)\)")

@dataclass
class ExportConfig:
    vault_root: Path
    source_md: Path
    target_repo: Path
    lectures_dir: Path          # target_repo / lectures
    assets_images_dir: Path     # target_repo / assets/images
    copy_mode: str              # "copy" or "move"
    avoid_overwrite: bool
    dry_run: bool

def is_image_path(p: str) -> bool:
    # Strip fragment/query if any
    p2 = p.split("#", 1)[0].split("?", 1)[0].strip()
    return Path(p2).suffix.lower() in IMAGE_EXTS

def sanitize_link_path(p: str) -> str:
    # Remove surrounding quotes and trim
    p = p.strip().strip('"').strip("'")
    # Remove <...> wrapper sometimes used in MD
    if p.startswith("<") and p.endswith(">"):
        p = p[1:-1].strip()
    return p

def safe_dest(dest_dir: Path, filename: str, avoid_overwrite: bool) -> Path:
    dest = dest_dir / filename
    if not avoid_overwrite or not dest.exists():
        return dest
    stem = dest.stem
    suf = dest.suffix
    i = 1
    while True:
        cand = dest_dir / f"{stem}_{i}{suf}"
        if not cand.exists():
            return cand
        i += 1

def find_file_by_name(root: Path, filename: str) -> Optional[Path]:
    for fp in root.rglob(filename):
        if fp.is_file():
            return fp
    return None

def resolve_image_source(
    vault_root: Path,
    source_md: Path,
    raw_path: str,
) -> Optional[Path]:
    """
    Resolve an image reference to an absolute file path in the vault.
    Handles:
      - relative paths from the md location
      - repo/vault-relative paths
      - plain filenames (search)
    """
    raw_path = sanitize_link_path(raw_path)
    # Remove Obsidian alias/size: "file.png|300"
    raw_path = raw_path.split("|", 1)[0].strip()

    # If not image, bail
    if not is_image_path(raw_path):
        return None

    p = Path(raw_path)

    # 1) path relative to md file
    cand1 = (source_md.parent / p).resolve()
    if cand1.exists() and cand1.is_file():
        return cand1

    # 2) path relative to vault root
    cand2 = (vault_root / p).resolve()
    if cand2.exists() and cand2.is_file():
        return cand2

    # 3) filename search in vault
    cand3 = find_file_by_name(vault_root, p.name)
    if cand3:
        return cand3.resolve()

    return None

def ensure_dirs(cfg: ExportConfig) -> None:
    if cfg.dry_run:
        return
    cfg.lectures_dir.mkdir(parents=True, exist_ok=True)
    cfg.assets_images_dir.mkdir(parents=True, exist_ok=True)

def copy_or_move(src: Path, dest: Path, mode: str, dry_run: bool) -> None:
    if dry_run:
        return
    if mode == "move":
        shutil.move(str(src), str(dest))
    else:
        shutil.copy2(str(src), str(dest))

def make_repo_relative_image_link(exported_md_path: Path, target_img_path: Path) -> str:
    # We want link in exported md relative to its location.
    rel = os.path.relpath(target_img_path.resolve(), exported_md_path.parent.resolve()).replace("\\", "/")
    return rel

def export_note(cfg: ExportConfig) -> Tuple[Path, int, int]:
    """
    Returns:
      exported_md_path, rewritten_link_count, copied_image_count
    """
    ensure_dirs(cfg)

    src_md = cfg.source_md.resolve()
    if not src_md.exists():
        raise FileNotFoundError(f"Source md not found: {src_md}")

    # Destination md path
    exported_md_path = (cfg.lectures_dir / src_md.name).resolve()

    # Read original content
    original = src_md.read_text(encoding="utf-8", errors="ignore")

    # Track image mapping: original reference token -> (dest filename)
    # But multiple refs can point to same file; we'll map by absolute src path.
    img_map: Dict[Path, Path] = {}
    copied = 0
    rewrites = 0

    # 1) Collect references from both syntaxes
    # We'll build a list of (kind, full_match, inner_or_path, alt_text_if_any)
    refs: List[Tuple[str, str, str, str]] = []

    for m in OBSIDIAN_EMBED_RE.finditer(original):
        inner = m.group(1).strip()
        token = inner.split("|", 1)[0].strip()
        if is_image_path(token):
            refs.append(("obsidian", m.group(0), token, ""))

    for m in MARKDOWN_IMAGE_RE.finditer(original):
        alt = m.group(1)
        path = sanitize_link_path(m.group(2))
        if is_image_path(path):
            refs.append(("markdown", m.group(0), path, alt))

    # 2) Copy/move images into assets/images and build a rewriting function
    def get_or_create_dest_for(src_img: Path) -> Path:
        nonlocal copied
        if src_img in img_map:
            return img_map[src_img]
        dest = safe_dest(cfg.assets_images_dir, src_img.name, cfg.avoid_overwrite)
        if not dest.exists():
            copy_or_move(src_img, dest, cfg.copy_mode, cfg.dry_run)
            copied += 1
        img_map[src_img] = dest
        return dest

    # We will rewrite in two passes using regex subs to avoid offset issues.
    # A) Rewrite Obsidian embeds
    def obsidian_repl(match: re.Match) -> str:
        nonlocal rewrites
        inner = match.group(1).strip()
        token = sanitize_link_path(inner.split("|", 1)[0].strip())
        if not is_image_path(token):
            return match.group(0)

        src_img = resolve_image_source(cfg.vault_root, cfg.source_md, token)
        filename = Path(token).name

        if src_img and src_img.exists():
            dest_img = get_or_create_dest_for(src_img)
            link = make_repo_relative_image_link(exported_md_path, dest_img)
            rewrites += 1
            return f"![]({link})"
        else:
            # still rewrite to assets/images/filename (best-effort)
            dest_img = (cfg.assets_images_dir / filename).resolve()
            link = make_repo_relative_image_link(exported_md_path, dest_img)
            rewrites += 1
            return f"![]({link})"

    rewritten = OBSIDIAN_EMBED_RE.sub(obsidian_repl, original)

    # B) Rewrite Markdown images (normalize to assets/images, and also copy those images)
    def markdown_repl(match: re.Match) -> str:
        nonlocal rewrites
        alt = match.group(1)
        path = sanitize_link_path(match.group(2))
        if not is_image_path(path):
            return match.group(0)

        src_img = resolve_image_source(cfg.vault_root, cfg.source_md, path)
        filename = Path(path.split("|", 1)[0].strip()).name

        if src_img and src_img.exists():
            dest_img = get_or_create_dest_for(src_img)
            link = make_repo_relative_image_link(exported_md_path, dest_img)
            rewrites += 1
            return f"![{alt}]({link})"
        else:
            dest_img = (cfg.assets_images_dir / filename).resolve()
            link = make_repo_relative_image_link(exported_md_path, dest_img)
            rewrites += 1
            return f"![{alt}]({link})"

    rewritten = MARKDOWN_IMAGE_RE.sub(markdown_repl, rewritten)

    # 3) Write exported md
    if not cfg.dry_run:
        # Copy md first (for timestamps etc.) then overwrite content with rewritten
        shutil.copy2(str(src_md), str(exported_md_path))
        exported_md_path.write_text(rewritten, encoding="utf-8")

    return exported_md_path, rewrites, copied

def main():
    ap = argparse.ArgumentParser(description="Export one Obsidian note to a separate repo: copy md, collect images, rewrite image links.")
    ap.add_argument("--vault", required=True, help="Path to Obsidian vault root")
    ap.add_argument("--md", required=True, help="Path to source markdown note inside vault")
    ap.add_argument("--repo", required=True, help="Path to target repo root (cmu-graphics-notes)")
    ap.add_argument("--lectures-dir", default="lectures", help="Directory in repo for exported markdown (default: lectures)")
    ap.add_argument("--assets-images-dir", default="assets/images", help="Directory in repo for images (default: assets/images)")
    ap.add_argument("--move", action="store_true", help="Move images instead of copying (default: copy)")
    ap.add_argument("--avoid-overwrite", action="store_true", help="If filename exists, create name_1, name_2, ...")
    ap.add_argument("--dry-run", action="store_true", help="Preview without writing/copying")
    args = ap.parse_args()

    cfg = ExportConfig(
        vault_root=Path(args.vault).expanduser().resolve(),
        source_md=Path(args.md).expanduser().resolve(),
        target_repo=Path(args.repo).expanduser().resolve(),
        lectures_dir=(Path(args.repo).expanduser().resolve() / args.lectures_dir),
        assets_images_dir=(Path(args.repo).expanduser().resolve() / args.assets_images_dir),
        copy_mode="move" if args.move else "copy",
        avoid_overwrite=args.avoid_overwrite,
        dry_run=args.dry_run,
    )

    if not cfg.vault_root.exists():
        raise SystemExit(f"Vault not found: {cfg.vault_root}")
    if not cfg.source_md.exists():
        raise SystemExit(f"Source md not found: {cfg.source_md}")
    if not cfg.target_repo.exists():
        raise SystemExit(f"Target repo not found: {cfg.target_repo}")

    exported_md, rewrites, copied = export_note(cfg)

    print("✅ Export complete")
    print(f"- Exported md: {exported_md}")
    print(f"- Rewritten image links: {rewrites}")
    print(f"- Images {cfg.copy_mode}d: {copied}")
    if cfg.dry_run:
        print("(dry-run: no files modified)")

if __name__ == "__main__":
    main()
