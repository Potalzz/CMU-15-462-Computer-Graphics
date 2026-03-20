>This post is a personal study note based on CMU 15-462 (Computer Graphics) 2020/fall lectures.
>All original lecture slides and videos are copyrighted by the instructors.

강의에 대한 정보와 자료는 아래 홈페이지에서 확인할 수 있습니다.
https://15462.courses.cs.cmu.edu/fall2020/home

본 포스팅은 강의 내용을 바탕으로 하되, 이해를 돕기 위해 별도의 자료 조사와 개념 정리를 덧붙여 작성했습니다. 따라서 원 강의 내용에 추가적으로 개인적인 학습 자료가 다수 포함되어 있습니다.

---

## First Hit Problem
![](../assets/images/Pasted%20image%2020260307183026.png)
**광선이 처음으로 부딪히는 삼각형 찾기.**

모든 삼각형 순회하면 가능하지만, 시간 복잡도가 O(N)으로 너무 비효율적.

## Bounding Box

오브젝트를 둘러싼 바운딩 박스를 만들고 계산.
- 레이가 박스를 빗나가면 내부 프리미티브는 검사할 필요가 없다. $O(1)$
- 하지만 레이가 박스를 맞으면 결국 모든 프리미티브를 검사한다. 최악의 경우는 여전히 $O(N)$

결론은 계층화를 통해 같은 전략을 한 번이 아니라 여러 번 적용.

---

## Bounding volume hierarchy(BVH)

### 정의
Bounding Volume Hierarchy(BVH)는 프리미티브 집합을 재귀적으로 둘로 나누어 트리를 만든다.
- 리프 노드: 소수의 프리미티브 리스트를 보관한다.
- 내부 노드: 서브트리 전체를 감싸는 바운딩 박스를 보관한다.
![](../assets/images/Pasted%20image%2020260307183747.png)

BVH는 프리미티브를 서로 겹치지 않는 집합으로 분할한다.  
단, 자식 박스는 공간에서 겹칠 수 있다.
겹침이 클수록 같은 레이가 두 자식을 모두 방문할 확률이 커진다.
![](../assets/images/Pasted%20image%2020260307183810.png)

### BVH로 first hit 찾기

탐색 로직은 다음과 같다.
1. 노드의 박스와 레이를 교차 테스트한다.
2. miss면 서브트리를 통째로 가지치기한다.
3. hit이고 리프면 리프 내부 프리미티브만 검사한다.
4. hit이고 내부 노드면 자식으로 재귀한다.

```c++
struct BVHNode {
    bool leaf; // am I a leaf node?
    BBox bbox; // min/max coords of enclosed primitives
    BVHNode* child1; // “left” child (could be NULL)
    BVHNode* child2; // “right” child (could be NULL)
    Primitive* primList; // for leaves, stores primitives
};

struct HitInfo {
    Primitive* prim; // which primitive did the ray hit?
    float t; // at what t value?
};

void find_closest_hit(Ray* ray, BVHNode* node, HitInfo* closest) {
    // test ray against node’s bounding box
    HitInfo hit = intersect(ray, node->bbox);
    if (hit.prim == NULL || hit.t > closest.t))
    	return; // don’t update the hit record
    
    if (node->leaf) {
        for (each primitive p in node->primList) {
        	hit = intersect(ray, p);
            if (hit.prim != NULL && hit.t < closest.t) {
                closest.prim = p;
                closest.t = t;
            }
    	}
    } else {
    	find_closest_hit(ray, node->child1, closest);
    	find_closest_hit(ray, node->child2, closest);
	}
}
```


### front-to-back traversal

두 자식 박스의 교차 $t$를 비교해 더 가까운 자식부터 방문한다.

- 먼저 방문한 서브트리에서 작은 $t_{\text{closest}}$를 찾으면,
- 두 번째 자식은 박스 교차 $t$가 $t_{\text{closest}}$보다 작을 때만 방문한다.

조기 종료 목적.  
단, 첫 자식 박스를 통과해도 실제 프리미티브 hit이 없을 수 있다. 그래서 조건부로 두 번째 자식을 검사한다.

```c++
void find_closest_hit(Ray* ray, BVHNode* node, HitInfo* closest) {
if (node->leaf) {
    for (each primitive p in node->primList) {
        (hit, t) = intersect(ray, p);
        if (hit && t < closest.t) {
            closest.prim = p;
            closest.t = t;
    	}
	}
} else {
    HitInfo hit1 = intersect(ray, node->child1->bbox);
    HitInfo hit2 = intersect(ray, node->child2->bbox);
    BVHNode* first = (hit1.t <= hit2.t) ? child1 : child2;
    BVHNode* second = (hit1.t <= hit2.t) ? child2 : child1;
    find_closest_hit(ray, first, closest);
    if (second child’s t is closer than closest.t)
    	find_closest_hit(ray, second, closest);
}
```

---

## BVH 품질과 SAH

### 좋은 분할의 직관

![](../assets/images/Pasted%20image%2020260307201704.png)
균형 잡힌 프리미티브 개수 분할이 항상 좋은 것은 아니다.

좋은 분할은 다음을 지향한다.
- 자식 박스가 타이트해야 한다.
- 자식 박스 겹침이 작아야 한다.
- 박스 내부의 빈 공간을 줄여야 한다.

### 비용 모델

리프 노드 비용은 단순 합이다.
$$
C = \sum_{i=1}^{N} C_{\text{isect}}(i) \approx N C_{\text{isect}}
$$

내부 노드는 기대 비용으로 근사한다.

$$
C = C_{\text{trav}} + p_A C_A + p_B C_B
$$

- $C_{\text{trav}}$: 내부 노드 traversal 비용(박스 교차 등)
- $p_A, p_B$: 레이가 자식 박스를 칠 확률
- $C_A, C_B$: 자식 서브트리에서의 기대 비용

자식 비용은 프리미티브 개수로 근사하는 경우가 많다.

$$
C_A \approx N_A C_{\text{isect}}, \quad C_B \approx N_B C_{\text{isect}}
$$

**Q. $p_A, p_B$는 어떻게 구할까 ?**

### Surface Area Heuristic

볼록 객체 $B$ 안의 볼록 객체 $A$에 대해, $B$를 맞힌 랜덤 레이가 $A$도 맞힐 조건부 확률을 표면적 비율로 근사한다.
$$
P(\text{hit}A \mid \text{hit}B) = \frac{S_A}{S_B}
$$

표면적 휴리스틱(Surface Area Heuristic)으로 이어짐.

$$
C \approx C_{\text{trav}} + \frac{S_A}{S_N} N_A C_{\text{isect}} + \frac{S_B}{S_N} N_B C_{\text{isect}}
$$

로 둔다. 여기서 $S_N$은 부모 박스 표면적.

SAH의 가정
- 레이는 랜덤 분포
- occlusion 없음

현실에서 완전히 맞지 않는다. 그래도 실용적으로 잘 동작한다는 경험적 근거가 있다.

---

## BVH 구축: 축정렬 분할과 버킷 근사

### 축정렬 분할

![](../assets/images/Pasted%20image%2020260307202219.png)
탐색 공간을 제한한다.

- 축 $x,y,z$ 중 하나를 고른다.
- 해당 축의 split plane을 정한다.
- 프리미티브 centroid가 어느 쪽에 있는지로 좌/우 집합을 나눈다.

후보 split이 많다. 따라서 근사를 사용한다.

### 버킷 기반 근사

![](../assets/images/Pasted%20image%2020260307202237.png)
공간 범위를 $B$개의 버킷으로 나누고, $B-1$개의 split만 평가한다. 보통 $B < 32$다.

절차 요약.
1. 각 축에 대해 버킷 배열을 초기화한다.
2. 각 프리미티브 $p$에 대해
   - centroid로 버킷 인덱스 $b$를 정한다.
   - $b$의 bbox를 $p$의 bbox와 union한다.
   - $b$의 primitive count를 증가시킨다.
3. $B-1$개의 후보 split에 대해 SAH 비용을 계산하고 최저 비용을 선택한다.
4. 선택된 split으로 재귀한다. 또는 충분히 작으면 리프로 만든다.

### 문제 사례
![](../assets/images/Pasted%20image%2020260307202326.png)
입력 분포가 나쁘면 분할이 실패하거나 품질이 떨어진다.

- 모든 centroid가 동일한 경우: 한쪽에만 몰린다.
- bbox가 거의 동일한 경우: 두 자식을 모두 방문하기 쉽다.
- 겹침이 큰 경우: 가지치기 효과가 약하다.

---

## 프리미티브 분할과 공간 분할
![](../assets/images/Pasted%20image%2020260307202405.png)
공간 가속 구조는 크게 두 계열이다.

**프리미티브 분할**
- 구조프리미티브 집합을 분할한다.
- BVH가 대표.
- 집합은 서로 disjoint.
- 박스는 공간적으로 겹칠 수 있다.

**공간 분할 구조**
- 공간 영역을 분할한다.
- uniform grid, KD tree가 대표.
- 공간은 서로 disjoint.
- 프리미티브는 여러 영역에 중복 포함될 수 있다.

---

## KD tree
![](../assets/images/Pasted%20image%2020260307202547.png)
KD tree는 공간을 축정렬 split plane으로 재귀 분할한다.

특징은 traversal을 front-to-back으로 수행할 수 있다는 점이다.  
따라서 첫 hit이 곧 closest hit이 된다. 조기 종료가 강하다.

### 프리미티브 중복
![](../assets/images/Pasted%20image%2020260307202615.png)
공간 분할에서는 프리미티브가 여러 셀에 걸칠 수 있다.

한 리프에서 큰 삼각형과 교차를 발견해도, 그 교차점이 현재 리프 셀 바깥일 수 있다.  
더 앞쪽 셀에서 더 가까운 교차가 존재할 수 있다.

해결은 단순함.
- 교차점이 현재 리프 셀 내부에 있을 때만 유효 hit으로 인정한다.

캐싱이나 mailboxing으로 반복 테스트 부작용을 줄일 수 있음.

---

## Uniform Grid
![](../assets/images/Pasted%20image%2020260307202818.png)
Uniform grid는 공간을 동일 크기의 복셀(voxel)로 나눈다. 각 셀은 겹치는 프리미티브 리스트를 가진다.

레이는 3D DDA 방식으로 셀을 순서대로 통과한다. 레이가 지나는 셀의 프리미티브만 검사한다. 구현은 효율적으로 만들 수 있다.

### 해상도 선택이 핵심

- 셀이 너무 적으면 brute-force로 퇴화한다.
- 셀이 너무 많으면 빈 셀 traversal 비용이 커진다.

![](../assets/images/Pasted%20image%2020260307202840.png)
실용적 휴리스틱은 복셀 수를 프리미티브 수와 비슷하게 두는 것이다. 균일 분포 가정.

슬라이드에서는 이때 교차 비용 스케일을 $O(N^{1/3})$로 언급한다.  
$N^{1/3}$과 $\log N$의 성장률 비교가 중요한 질문이다.

### 잘 동작하는 경우와 실패하는 경우
![](../assets/images/Pasted%20image%2020260307202927.png)
균일한 크기와 분포의 프리미티브에 강하다. 지형(height field), 잔디 같은 장면이 대표.

![](../assets/images/Pasted%20image%2020260307203003.png)
비균일 분포에는 취약하다. 장면 범위는 큰데 디테일이 한 곳에 몰리면 문제가 커진다. teapot in a stadium 사례.

---

## Quad-tree와 Octree
![](../assets/images/Pasted%20image%2020260307203037.png)
Quad-tree는 2D에서 4분할, Octree는 3D에서 8분할한다.

- uniform grid처럼 구축이 단순하다.
- uniform grid보다 적응성이 있다. 디테일이 몰린 영역만 더 깊게 쪼갠다.
- KD tree만큼 분할 위치를 정교하게 최적화하기는 어렵다. 성능은 보통 KD tree보다 낮다.

---

## 선택 가이드

구조 선택은 장면 분포, 질의 유형, 구축 비용 amortization 여부에 달린다.

- **BVH**
  - 장점: 노드 수가 프리미티브 수에 의해 제한된다. 변형이 있어도 refit이 가능해 갱신이 비교적 단순하다.
  - 단점: 박스 겹침이 크면 성능이 떨어진다. 구축 비용이 존재한다.
- **KD tree**
  - 장점: front-to-back traversal로 첫 hit이 closest hit이다. 조기 종료에 유리하다.
  - 단점: 프리미티브 중복 포함, 반복 교차가 발생한다.
- **Uniform grid**
  - 장점: 구축이 매우 저렴하다. 균일 분포에서 빠르다.
  - 단점: 해상도 선택이 민감하다. 비균일 분포에서 급격히 악화한다.
- **Octree**
  - 장점: 구축이 단순하고 적응적이다.
  - 단점: 분할 자유도가 낮아 KD tree 대비 손해가 날 수 있다.

---

## 계층적 가속의 그래픽스 응용

공간 자료구조는 레이 트레이싱만의 도구가 아님. 다양한 geometry query에 반복적으로 등장한다.

- **Geometry**
	- 내부-외부 테스트(eg. meshing)
	- Closest point tests
- **Animation/Simulation**
	- particle systems
	- N-body
	- Barnes-Hut
	- fast multipole method
- **Rendering**
	- visibility
	- physically-based ray tracing

---

## 렌더링 관점: Rasterization과 Ray Casting

가시성(visibility)은 두 질문으로 쪼갤 수 있다.

- coverage: 삼각형이 어떤 샘플을 덮는가
- occlusion: 그 샘플에서 어떤 삼각형이 가장 가까운가

### Rasterization

이전에 나온 레스터화 알고리즘에서는 삼각형 순서로 진행한다.
depth buffer로 occlusion을 처리한다.

고성능 구현에서는 2D 전 화면이 아니라 삼각형의 2D bounding box 범위로 테스트를 제한한다.

### Ray Casting

샘플 순서로 진행한다. 각 샘플에서 3D 레이를 쏘고, 가장 작은 $t$를 갖는 교차를 선택한다.

여기서 병목은 레이당 교차 테스트 수다. 그래서 BVH, KD tree 같은 가속 구조가 핵심이 된다.

### 차이 요약

- **Rasterization**
  - 삼각형 순서
  - depth buffer 사용
  - 장면 전체를 메모리에 올리지 않아도 된다. 매우 큰 장면 스트리밍에 유리하다.
- **Ray casting**
  - 샘플 순서
  - 현재 레이의 최단 hit만 유지하면 된다.
  - 투명 물체처럼 레이 경로상 순서가 중요한 처리가 자연스럽다.
  - 장면과 가속 구조를 메모리에 두는 경우가 많다.
  - 성능이 장면 분포와 가속 구조 품질에 크게 좌우된다.

---

## 요약
- 단일 AABB는 miss에서만 큰 이득이다. hit에서는 $O(N)$이 남는다.
- BVH는 바운딩 박스를 계층화해 가지치기를 만든다.
- SAH는 분할 비용을 표면적 비율로 근사해 BVH 품질을 높인다.
- KD tree는 공간 분할로 front-to-back 조기 종료가 강하다. 대신 중복 교차가 생긴다.
- uniform grid는 균일 분포에서 강하지만 비균일 분포에서 위험하다.
- 최종 선택은 장면 분포와 질의 패턴, 구축 비용 amortization에 달려 있다.
