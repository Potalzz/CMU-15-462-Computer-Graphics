>This post is a personal study note based on CMU 15-462 (Computer Graphics) 2020/fall lectures.
>All original lecture slides and videos are copyrighted by the instructors.

강의에 대한 정보와 자료는 아래 홈페이지에서 확인할 수 있습니다.
https://15462.courses.cs.cmu.edu/fall2020/home

본 포스팅은 강의 내용을 바탕으로 하되, 이해를 돕기 위해 별도의 자료 조사와 개념 정리를 덧붙여 작성했습니다. 따라서 원 강의 내용에 추가적으로 개인적인 학습 자료가 다수 포함되어 있습니다.

---

## Geometry Processing이 다루는 문제

전통적인 디지털 신호 처리(DSP)가 이미지·오디오·비디오 같은 정규 격자 신호를 다뤘다면, Geometry Processing은 형상 자체를 신호로 보고 처리한다.
업샘플링, 다운샘플링, 리샘플링, 필터링 같은 작업이 핵심 축.
![](../assets/images/Pasted%20image%2020260304001222.png)

### Geometry Processing의 대표 작업
**Reconstruction**
샘플(점군, 법선 포함 점, 다중 시점 이미지, CT/MRI 선적분 등)로부터 표면을 재구성한다.

**Upsampling**
- 해상도를 올린다. 메쉬에서는 subdivision이 대표적이다.

**Downsampling**
- 해상도를 내리면서 형태와 외관을 보존하려 한다. 메쉬에서는 edge decimation(간소화)이 대표적이다.

**Resampling/Remeshing**
- 요소 개수는 크게 바꾸지 않더라도 표본 분포를 바꿔 요소 품질을 높인다.

**Filtering**
- 노이즈 제거 혹은 중요한 특징 강조를 한다. 메쉬에서는 curvature flow, bilateral, spectral 계열이 등장한다.

**Compression**
- 기하(좌표)와 연결성(connectivity)을 함께 압축해야 한다.

**Shape analysis**
- 분할, 대응점, 대칭 등 의미 기반 분석을 한다.
  

---

## Remeshing을 리샘플링으로 보기

![](../assets/images/Pasted%20image%2020260304181253.png)
리샘플링 관점에서 remeshing을 보면 aliasing 문제가 그대로 등장한다.
샘플링이 나쁘면 재구성된 표면이 원래 신호와 다른 인상을 준다. 언더샘플링은 디테일을 날리고, 오버샘플링은 성능과 메모리를 잡아먹는다.

---

## 좋은 메쉬의 조건

원래 형태에 대해 근사해야 함. 곡률이 큰 곳에 더 많은 표본을 둠.

하지만 정점이 원래 표면 위에 정확히 놓여 있어도 문제가 생긴다.
삼각형의 연결과 방향에 따라 법선, 면적, 곡률 추정이 크게 틀어질 수 있다.
![](../assets/images/Pasted%20image%2020260304181725.png)

### 삼각형 모양

각이 $60^\circ$에 가까운 삼각형이 선호됨.

더 세련된 조건으로 Delaunay 삼각분할이 있다.
![](../assets/images/Pasted%20image%2020260304182041.png)

### 정점 차수의 규칙성

삼각형 메쉬에서 이상적 규칙 격자는 차수(degree) 6에 가깝다. 사각형 메쉬는 차수 4가 기준이다.  
![](../assets/images/Pasted%20image%2020260304182104.png)

정점 차수 규칙성은 다음과 연결된다.
- 삼각형 모양: 차수가 극단적으로 크거나 작으면 좋은 각을 만들기 어렵다.
- 계산 규칙성: 연결성이 규칙적이면 메모리 접근과 병렬화가 단순해진다.
- subdivision 품질: 비정상 차수 정점 주변에서 한계 곡면이 아티팩트를 만들 수 있다.

---

## 업샘플링: Subdivision (다양한 Subdivision 기법들)

설계 시 고려사항
- interpolating vs approximating: 원래 정점을 반드시 지나는가, 아니면 근방으로 수렴하는가.
- 한계 곡면 연속성: $C^1$, $C^2$ 등 미분 가능성.
- 비정상 차수 정점에서의 거동.



### Catmull-Clark subdivision
![](../assets/images/Pasted%20image%2020260304182400.png)
Catmull-Clark은 일반 폴리곤 메쉬를 반복 적용을 통해 사각형 메쉬로 만든다. 절차는 다음과 같다.

![](../assets/images/Pasted%20image%2020260304182854.png)
Face 좌표 -> Edge 좌표 -> Vertex 좌표 순으로 계산

- Face point: 각 면의 정점 평균을 사용한다.
- Edge point: 엣지 양 끝점과 인접한 두 face point의 평균을 사용한다.
- Vertex update: 정점 차수를 $n$이라 할 때
  - $Q$: 주변 face point의 평균
  - $R$: 주변 edge point의 평균
  - $S$: 원래 정점 좌표
  - 갱신 좌표는 $S' = \frac{Q + 2R + (n-3)S}{n}$ 이다.

사각형 메쉬에서는 비정상 정점이 적게 유지되어 법선이 매끄럽게 변하는 경향이 있다. 반면 삼각형 메쉬에 억지로 적용하면 차수 4 기준에서 벗어난 정점이 대량 발생해 법선이 불안정해질 수 있다.  

**quad mesh에서의 결과**
![](../assets/images/Pasted%20image%2020260304183010.png)

**triangle mesh에서의 문제**
![](../assets/images/Pasted%20image%2020260304183032.png)

### Loop subdivision
>삼각형 메쉬 전용 대표 기법

비정상 정점에서 멀어지면 곡률 연속이 유지되는 $C^2$ 성질을 목표로 한다.  

**알고리즘**
- 각 삼각형을 4개로 분할한다. 각 엣지 중점에 새 정점을 만든다.

새 엣지 정점 좌표
- 엣지 $(v_i, v_j)$ 가 두 삼각형에 공유되고 반대 꼭짓점이 $(v_k, v_l)$ 라면  
  $v_{new} = \frac{3}{8}(v_i + v_j) + \frac{1}{8}(v_k + v_l)$ 이다.

기존 정점 좌표 갱신:

- 정점 차수가 $n$, 이웃이 $v_1,\dots,v_n$ 일 때  
  $u = \frac{3}{16}$ if $n=3$, else $u = \frac{3}{8n}$  
  $v' = (1 - nu)v + u \sum_{m=1}^{n} v_m$ 이다.

**Loop subdivision Edge**

![](../assets/images/Pasted%20image%2020260304184152.png)
먼저 원래 엣지를 임의의 순서로 모두 분할.

![](../assets/images/Pasted%20image%2020260304184156.png)
새 정점과 옛 정점을 잇는 새 엣지는 flip으로 뒤집음.

---

## 다운샘플링: Edge collapse simplification

![](../assets/images/Pasted%20image%2020260304184626.png)
메쉬를 단순화하기 위해 모서리를 반복적으로 축소

greedy 알고리즘 사용.
각 모서리에 비용을 부여하고 표면을 가장 적게 변경하는 비용이 가장 작은 엣지를 먼저 붕괴

Quadric Error Metric(QEM) 알고리즘을 사용하면 효율적으로 사용 가능.

### Quadric Error Metric의 정의
3D 모델의 폴리곤 개수를 줄여나가면서(메쉬 단순화) 원본 모델과 형태가 최대한 유사하도록 덜 중요한 부분부터 깎아내는 알고리즘


![](../assets/images/Pasted%20image%2020260304184933%201.png)
점 $x$ 와, 단위 법선 $n$ 을 가지며 점 $p$ 를 지나는 평면 사이의 거리(정확히는 법선 방향 투영)는
$dist(x) = \langle n, x-p \rangle$ 로 쓴다.  

여러 평면(혹은 삼각형들의 지지 평면) 집합에 대한 오류는 제곱거리 합으로 둔다.  
![](../assets/images/Pasted%20image%2020260304235423.png)
$Q(x) = \sum_{i=1}^{k} \langle n_i, x - p_i \rangle^2$  

이 값은 특정 지역의 삼각형 집합에 대해, 한 점이 그 지역을 얼마나 잘 근사하는지를 수치화한다.

### 동차좌표에서의 표현

동차좌표를 쓰면 QEM을 행렬 하나로 누적할 수 있다.  

- 점 $x=(x,y,z)$ 를 $u=(x,y,z,1)$ 로 확장한다.
- 평면은 $v=(a,b,c,d)$ 로 표현한다. 여기서 $(a,b,c)$ 는 법선, $d$ 는 오프셋이다.
- 부호 있는 평면 방정식은 $\langle u,v \rangle = ax + by + cz + d$ 로 쓴다.
- 제곱거리는 $\langle u,v\rangle^2 = u^T(vv^T)u$ 이다.

![](../assets/images/Pasted%20image%2020260305000547.png)
따라서 평면 하나의 quadric은 $K = vv^T$ 로 표현된다. 여러 평면에 대한 합은 단순히 행렬 합으로 누적된다. 즉  
$u^T K_1 u + u^T K_2 u = u^T (K_1 + K_2) u$  
이 성질 때문에 비용 누적이 싸다. 행렬 크기가 늘지 않는다.

### 엣지 collapse 비용과 최적 위치

엣지 $e_{ij}$ 의 두 끝점 정점에 대해 quadric을 $K_i, K_j$ 라고 하자. 엣지를 collapse해 새 정점으로 만들 때 지역 평면 집합을 동시에 근사해야 하므로  
$K_{ij} = K_i + K_j$ 로 합친다.  

가장 단순한 방식은 중점 $m$ 을 두고 비용을 $Q(m)=m^T K_{ij} m$ 로 두는 것이다. 더 나은 방식은 비용을 최소화하는 점 $x$ 를 직접 찾는 것이다.  
![](../assets/images/Pasted%20image%2020260305001120.png)

최소화는 이차함수 최소화로 환원된다. 동차좌표에서 마지막 성분이 1이라는 제약을 사용해, 미지수는 $x\in\mathbb{R}^3$ 만 남긴다. 행렬을 블록으로 쪼개면 다음 꼴이 된다.  
$x^T B x + 2 w^T x + d^2$  

기울기를 0으로 두고 미분을 하면,
$2Bx + 2w = 0$ 이고, 해는  
$x= -B^{-1} w$ 이다.  
![](../assets/images/Pasted%20image%2020260305001818.png)

여기서 $B$ 가 가역이 아니면 역행렬이 존재하지 않는다. 구현에서는 보통 다음 중 하나로 처리한다.
- 정규화 혹은 작은 항을 더해 가역성을 만든다.
- 후보 위치를 몇 개(중점, 두 끝점 등)로 제한해 그중 최소 비용을 선택한다.

이 부분은 수치 안정성 이슈다.

### 전체 알고리즘 요약
![](../assets/images/Pasted%20image%2020260305003001.png)
- 초기화
  - 각 삼각형 평면에서 $K$ 를 만든다.
  - 각 정점 $i$ 에 대해 incident triangle의 $K$ 를 더해 $K_i$ 를 만든다.
  - 각 엣지 $e_{ij}$ 에 대해 $K_{ij}=K_i+K_j$ 를 만들고, 최소화로 최적 위치 $x$ 와 비용 $K_{ij}(x)$ 를 계산한다.
- 반복
  - 비용이 최소인 엣지를 선택해 collapse한다.
  - 새 정점 quadric은 $K_{ij}$ 로 둔다.
  - 새 정점에 인접한 엣지들의 비용을 갱신한다.

효율 구현은 우선순위 큐가 자연스럽다.

### Flipped Triangles

collapse 후 새 정점 위치에 따라 일부 삼각형이 뒤집힐 수 있다.
법선이 반대 방향을 향하는 상황이다.
간단한 방법은 collapse 후보를 사전 차단하는 것이다.
붕괴 후 영향을 받는 인접 삼각형들의 법선 내적이 음수가 되면 해당 collapse를 버린다.  
![](../assets/images/Pasted%20image%2020260305002948.png)

---

## 고정된 해상도에서 품질 개선

삼각형 개수는 유지하면서 요소 품질을 올리는 문제다. remeshing이라고 부르는 맥락이다.

### Delaunay 성질로 유도하기: edge flip

평면 삼각분할에서, 한 엣지를 공유하는 두 삼각형의 반대각을 $\alpha,\beta$ 라 하자. $\alpha+\beta>\pi$ 이면 해당 엣지를 flip한다. 이 반복은 결국 Delaunay 삼각분할로 수렴한다.  
![](../assets/images/Pasted%20image%2020260305003135.png)

표면(3D)에서는 이론적 보장이 약해질 수 있으나, 일정 횟수 제한 등으로 충분히 쓸 만하다.
경험적 안정성에 기대는 부분

### 정점 차수 개선: edge flip

같은 edge flip을 다른 목표로 쓸 수도 있다. 네 정점 $i,j,k,l$ 에 대해 flip 전후 차수 변화가 생기므로, 차수 6에서의 총 편차가 줄어들면 flip한다.  
$|d_i-6| + |d_j-6| + |d_k-6| + |d_l-6|$  
![](../assets/images/Pasted%20image%2020260305003208.png)

이 과정은 비정상 차수가 특정 영역에 몰려 있을 때 주변으로 퍼뜨리는 효과가 있다. 규칙성이 확산되는 형태다.

### 더 둥근 삼각형: Laplacian smoothing

Delaunay가 항상 각을 $60^\circ$ 근처로 만들지는 않는다. 따라서 정점을 이웃 평균으로 당기는 Laplacian smoothing을 사용한다.  
![](../assets/images/Pasted%20image%2020260305003233.png)

평면에서 가장 단순한 갱신은  
$\bar v = \frac{1}{n}\sum_{m=1}^n v_m$, $\Delta v = \bar v - v$  
이고 $v \leftarrow v + \lambda \Delta v$ 로 쓴다.

표면 위에서는 법선 방향 이동이 기하를 망가뜨릴 수 있으므로 접평면 성분만 남긴다.  
$\Delta v_t = \Delta v - \langle \Delta v, n\rangle n$  
$ v \leftarrow v + \lambda \Delta v_t $  
이렇게 하면 원래 표면을 따라 미끄러지듯 이동한다. 방향 제어 문제다.

### Isotropic remeshing 알고리즘

위의 국소 연산들을 조합하면 요소 길이와 모양을 동시에 정렬하는 isotropic remeshing을 구성할 수 있다.  

목표는 삼각형의 크기와 모양을 가능한 균일하게 만드는 것이다. 평균 엣지 길이를 $\bar \ell$ 로 두면 반복 루프는 다음 네 단계로 구성된다.

- split: $\ell > \frac{4}{3}\bar \ell$ 인 엣지를 분할한다.
- collapse: $\ell < \frac{4}{5}\bar \ell$ 인 엣지를 붕괴한다.
- flip: 차수 개선을 위해 엣지를 뒤집는다.
- tangential smoothing: 정점을 접평면 방향으로 중심화한다.

반복 횟수와 목표 길이 설정이 품질을 좌우한다.

---

## 리샘플링의 위험과 원본 투영

리샘플링은 누적 오차를 만든다. 이미지도 다운샘플-업샘플을 반복하면 품질이 계속 떨어진다. 메쉬도 동일한 현상이 나타난다.  
![](../assets/images/Pasted%20image%2020260305003256.png)
![](../assets/images/Pasted%20image%2020260305003304.png)

가능한 대응은 원본 신호를 보존하고, 처리된 표본을 원본 표면에 다시 투영하는 것이다. 예를 들어 처리된 정점을 원본 메쉬의 최근접점으로 projection한다.  

그러면 다음 질문이 등장한다.

- 공간의 한 점에 대해 표면 위 최근접점을 어떻게 찾는가.
- 점이 표면 내부인지 외부인지 어떻게 판정하는가.
- 삼각형-삼각형 교차를 어떻게 계산하는가.
- 큰 메쉬에서 위 질의를 어떻게 가속하는가.

이 문제들은 geometric query와 가속구조로 이어진다.  

---

- 전제 확인: 비매니폴드 입력(팬이 아닌 핀)이면 많은 국소 연산이 실패한다. 전처리 필요다.
- 자료구조: halfedge 기반이면 split, collapse, flip을 일관되게 구현하기 쉽다.
- 목표 정의: 렌더링용 메쉬와 시뮬레이션용 메쉬의 품질 기준은 다르다. 기준을 먼저 고정한다.
- 수치 안정성: QEM 최소화에서 $B$ 특이(singular) 가능성을 반드시 처리한다.
- 품질 보호: edge collapse 시 뒤집힘 검사를 넣는다.
- 반복 리샘플링 최소화: 파이프라인 단계가 길면 원본 투영이나 제약 기반 최적화를 고려한다.

---

## 요약
- Geometry processing은 형상을 신호로 보고 업샘플링·다운샘플링·리샘플링을 수행한다.
- 좋은 메쉬는 위치 근사만이 아니라 법선, 요소 모양, 정점 차수 등 다양한 품질 조건을 함께 본다.
- 업샘플링은 subdivision으로 구현한다. 사각형은 Catmull-Clark, 삼각형은 Loop가 대표적이다.
- 다운샘플링은 edge collapse와 QEM이 강력하다. 동차좌표로 quadric을 행렬 하나에 누적한다.
- 고정 해상도 품질 개선은 edge flip, 차수 개선, tangential Laplacian smoothing을 조합해 isotropic remeshing으로 확장한다.
- 반복 리샘플링은 품질을 지속적으로 깎는다. 원본 투영은 한 가지 대응이며, geometric query 문제가 뒤따른다.
