This post is a personal study note based on CMU 15-462 (Computer Graphics) 2020/fall lectures.
>All original lecture slides and videos are copyrighted by the instructors.

강의에 대한 정보와 자료는 아래 홈페이지에서 확인할 수 있습니다.  
https://15462.courses.cs.cmu.edu/fall2020/home

본 포스팅은 강의 내용을 바탕으로 하되, 이해를 돕기 위해 별도의 자료 조사와 개념 정리를 덧붙여 작성했습니다. 따라서 원 강의 내용에 추가적으로 개인적인 학습 자료가 다수 포함되어 있습니다.

---

## 들어가며
이전 강의에서 아래 내요을 배움
- **rasterization**: primitive(특히 triangle)을 픽셀로 바꾸는 방법
- **transformations**: primitive를 공간에서 움직이는 방법

이 두 아이디어가 실제 파이프라인에서 어떻게 **맞물리는지**를 다룰 것임.
- 3D를 2D로 투영하는 **Perspective Projection(원근 투영)**,
- triangle 내부에서 attribute(색, 텍스처 좌표 등)를 자연스럽게 퍼뜨리는 **Interpolation(보간)**,
- 그리고 텍스처를 그럴듯하게 보이게 만들기 위한 **Texture Sampling/Filtering**

>**Texture Mapping(텍스처 매핑)**
>2D 이미지(texture)를 3D 표면(surface)에 붙이는 기법.
>하지만 그냥 $(u,v)$ 좌표 한 번 찍는 것만으로는 고퀄이 나오지 않음.
>**perspective + sampling** 때문에 aliasing이 쉽게 생기고, 이를 막기 위해 여러 필터링이 필요하다.

---

## Perspective Projection (원근 투영)
원근 투영은 멀리 있는 물체일수록 작아 보이는 효과.

그래픽스에서는 원근 투영 덕분에 현실 세계처럼 자연스러운 이미지를 만들 수 있다.  
하지만 항상 원근 투영만 쓰는 건 아니다.

지도/엔지니어링 도면/아이소메트릭 뷰 같은 곳에서는 크기/거리의 보존이 중요해서 **orthographic projection(직교 투영)** 같은 방식이 더 적합할 수 있다.

![](../assets/images/Pasted%20image%2020260301232515.png)

---

## Transformations: From Objects to the Screen
**변환 파이프라인 전체 다시 한 번 정리**
![](../assets/images/Pasted%20image%2020260301232626.png)

큰 흐름은 다음과 같다.
1. **World coordinates**: 원래 물체가 정의된 좌표계
2. **View coordinates**: 카메라 기준 좌표계 (카메라가 원점, $-z$를 본다고 가정)
3. **Clip coordinates**: view frustum을 표준 공간(단위 큐브)으로 옮긴 좌표계
4. **Normalized coordinates (NDC)**: perspective divide 이후 $[-1,1]^2$에 있는 2D 좌표
5. **Image coordinates**: $(W\times H)$ 픽셀 좌표계로 스케일/반전한 최종 좌표

>카메라를 움직인다는 말은 실제로는 모든 물체에 카메라 변환의 **역변환(inverse)** 을 적용한다는 뜻.

---

## Review: 간단한 카메라 변환
![](../assets/images/Pasted%20image%2020260301232733.png)

Q. 카메라가 $(4,2,0)$에 있고, 특정 방향을 보고 있을 때 물체를 **카메라가 원점에 있고 $-z$를 바라보는** 좌표계로 옮기려면?

핵심 아이디어는 두 단계다.

### 1) 카메라를 원점으로 옮기기 (translation)
카메라 관점에서 모든 물체는 **카메라 위치 기준으로** 표현되어야 한다.  
즉 물체의 모든 vertex에 카메라 위치를 빼면 된다.

$$
p_{view} = p_{world} - c
$$

여기서 $c=(4,2,0)$.

### 2) 카메라 시선 방향 맞추기 (rotation)
카메라가 바라보는 방향을 표준 방향($-z$)으로 정렬시키기 위해 회전이 필요하다.  
(슬라이드의 예시에서는 $y$축 회전 $\pi/2$가 등장)

---

## Camera looking in a different direction (일반화)
이제 카메라가 임의의 방향 $w\in\mathbb{R}^3$를 본다고 하자.

![](../assets/images/Pasted%20image%2020260301232931.png)

Q. 카메라가 원점에 있고, 시선 방향이 $w$일 때, **물체를 카메라가 $-z$를 보는 표준 좌표계**로 옮기는 회전은?

### 1) $w$에 직교하는 $u,v$ 만들기
- $w$에 대해 대충 up인 벡터를 하나 잡고
- Gram-Schmidt로 $w$에 직교하도록 정리한 뒤 정규화해서 $v$로 둔다.
- 마지막 축은 외적으로 만든다.

$$
u := w \times v
$$

이렇게 하면 $(u,v,w)$가 정규직교기저(orthonormal basis)가 된다.

### 2) 회전행렬 구성
슬라이드의 구성은 기본 좌표축이 어디로 가는지를 보여준다.

- $x$축 $\to u$
- $y$축 $\to v$
- $z$축 $\to -w$

결과적으로 아래 행렬이 나옴.
$$
R = \begin{bmatrix}
u_x & v_x & -w_x \\
u_y & v_y & -w_y \\
u_z & v_z & -w_z
\end{bmatrix}
$$

### 3) 실제로는 역변환을 물체에 적용
카메라를 변환하는 대신, 물체를 반대로 돌려야 한다.  
회전행렬은 직교행렬이라서 역행렬이 전치행렬이다.

$$
R^{-1} = R^T
$$

---

## View Frustum
View frustum은 **카메라가 볼 수 있는 3D 영역**
![](../assets/images/Pasted%20image%2020260301234429.png)
- left/right/top/bottom plane: 화면의 네 변에 해당
- near/far plane: **너무 가까운 것**과 **너무 먼 것**을 잘라내는 경계

---

## Clipping
클리핑은 view frustum 밖에 있는 primitive(주로 triangle)를 제거하는 과정이다.

Q. 왜 클리핑을 해야 할까?
- 픽셀 단위로 fragment를 버리는 건(= fine granularity) 비싸다.
- triangle 단위로 버리면(= coarse granularity) 레스터화 계산을 아예 안 해도 된다.

즉 보이지도 않는 triangle을 굳이 레스터화하지 않기 위해 클리핑을 함.

### 부분적으로 걸친 triangle은?
![](../assets/images/Pasted%20image%2020260301234617.png)
문제는 triangle이 frustum에 **부분적으로만** 걸칠 수도 있다는 것.  
이 경우 실제 파이프라인에서는 polygon을 잘라낸 뒤 **triangle들로 다시 쪼갠다.**

**왜 꼭 triangle로 쪼개나?**
- GPU 레스터화 파이프라인이 “triangle”을 가장 빠르게 처리하도록 설계되어 있기 때문.

---

## Near/Far Clipping이 중요한 이유
멀리 있는 것도 보이는데 왜 far plane을 두지? 같은 의문이 생길 수 있다.

근본적인 이유는 두 가지.
### 1) 레스터화/투영의 골칫거리 제거
어떤 triangle은 카메라 앞/뒤를 동시에 가로지를 수 있음.
이런 경우 눈 뒤에 있는 fragment를 어떻게 처리할지 같은 문제가 생긴다.  
near plane으로 너무 가까운 영역을 잘라두면 파이프라인이 훨씬 단순해진다.

### 2) Depth buffer 정밀도(precision) 문제
다음 강의에서 배우는 Z-buffer(Depth buffer)는 깊이를 보통 float로 저장한다.  
float는 0 근처에서 상대적으로 정밀도가 높고, 큰 값으로 갈수록 표현이 거칠어진다.

![](../assets/images/화면%20기록%202026-03-01%20오후%2011.49.08.gif)
- near가 너무 작고 far가 너무 크면(예: $10^{-5}$ ~ $10^5$)
  - depth 값의 유효 정밀도가 부족해져
  - 두 표면이 거의 같은 깊이를 가질 때 어떤 게 앞인지 계속 바뀌는 **Z-fighting**이 발생한다.
- 따라서 일반적으로는 **near는 가능한 크게**, **far는 필요한 만큼만** 잡는 게 유리하다.

---

## Mapping Frustum to Unit Cube
2D로 투영하기 전에, view frustum을 표준 큐브 $[-1,1]^3$로 매핑한다.

![](../assets/images/Pasted%20image%2020260301235118.png)
Q. 왜 굳이 $[-1,1]^3$로 옮길까?
- 클리핑이 쉬워짐. 좌표가 $[-1,1]$ 범위를 벗어나면 그냥 버리면 된다.
- 하드웨어/알고리즘을 단순화할 수 있다.

### 이 매핑을 행렬로 표현할 수 있나?
가능함.

강의에서는 frustum의 8개 꼭짓점 $x_1,\dots,x_8$을 큐브 꼭짓점 $y_1,\dots,y_8$에 대응시키고
$$
A x_i = y_i
$$
를 만족하는 행렬 **A를 미지수로 두고** 푸는 방식으로 설명한다.  
(보통은 $Ax=y$에서 $x$를 풀지만, 여기서는 **행렬 $A$ 자체를 푸는** 셈)

![](../assets/images/Pasted%20image%2020260302000505.png)

>직교(orthographic) 케이스에서는 “translate로 중심을 원점으로 옮기고 → scale로 크기를 2로 맞춘다”는 형태로 역설계가 가능하다.

---

## Perspective Transform Matrix
가장 단순한 원근 투영은 다음과 같다.

- 3D 점 $(x,y,z)$를 2D로 보낼 때
$$
(x,y,z) \mapsto \left(\frac{x}{z},\frac{y}{z}\right)
$$

동차좌표(homogeneous coordinates)에서는 마지막에 $z$로 나눈다는 걸 행렬로 표현하기 위해 $w$에 $z$를 복사해두고, 마지막에 **perspective divide**를 한다.

즉
$$
(x,y,z,1) \xrightarrow{P} (x',y',z',w') \xrightarrow{divide} \left(\frac{x'}{w'}, \frac{y'}{w'}, \frac{z'}{w'}\right)
$$

![](../assets/images/Pasted%20image%2020260302000613.png)
일반적인 frustum $(l,r,b,t,n,f)$에 대해 perspective matrix는 보통 아래 형태로 쓴다.

$$
P =
\begin{bmatrix}
\frac{2n}{r-l} & 0 & \frac{r+l}{r-l} & 0 \\
0 & \frac{2n}{t-b} & \frac{t+b}{t-b} & 0 \\
0 & 0 & -\frac{f+n}{f-n} & -\frac{2fn}{f-n} \\
0 & 0 & -1 & 0
\end{bmatrix}
$$

(부호/세부 형태는 좌표계 관례(OpenGL/DirectX, $+z$ vs $-z$ 등)에 따라 조금씩 달라질 수 있음)

---

## Review: Screen Transformation
투영까지 끝나면 NDC(Normalized Device Coordinates)는 보통 $[-1,1]\times[-1,1]$ 범위의 2D 좌표다.  
이걸 실제 이미지 픽셀 좌표 $(W\times H)$로 바꾸는 마지막 변환이 **screen transform**이다.

![](../assets/images/Pasted%20image%2020260302000925.png)
1. 이미지 좌표계는 보통 $y$가 아래로 증가하므로 **x축에 대한 반사(reflection)**  
2. $(-1,-1)$이 원점이 되도록 **translate by $(1,1)$**
3. 가로/세로 크기에 맞추도록 **scale by $(W/2, H/2)$**

---

## Interpolation: triangle 위에서 값을 어떻게 퍼뜨릴까?
레스터화는 triangle 내부의 샘플 $(x,y)$를 찍어 fragment를 만든다.  
그런데 우리는 단순히 **coverage**만 계산하는 게 아니라,
- 색(color)
- 법선(normal)
- 텍스처 좌표 $(u,v)$
- (등등의 여러 값들..)

처럼 **vertex에 붙어있는 attribute**를 fragment로 옮겨야 한다.

---

## Linear Interpolation in 1D
![](../assets/images/Pasted%20image%2020260302002111.png)
점 $x_i, x_{i+1}$에서의 샘플 값이 $f_i, f_{i+1}$일 때,  
사이를 잇는 가장 간단한 보간은 선형 보간이다.

$$
t := \frac{x - x_i}{x_{i+1} - x_i} \in [0,1]
$$

$$
\hat f(x) = (1-t)f_i + t f_{i+1}
$$

---

## Linear Interpolation in 2D

triangle의 세 vertex에서의 값을 맞추는 2D 보간은  
결국 **affine function**(평면)을 하나 찾는 문제다.
$$
\hat f(x,y) = ax + by + c
$$

![](../assets/images/Pasted%20image%2020260302002350.png)
세 점에서의 값을 맞추면 $a,b,c$에 대한 선형 시스템이 생기는데 너무 복잡함.

그래서 그래픽스에서는 이걸 더 기하학적인 방식으로 푼다.

---

## Barycentric Coordinates
triangle $(i,j,k)$ 내부의 어떤 점 $x$에 대해

$$
x = \phi_i x_i + \phi_j x_j + \phi_k x_k,\quad
\phi_i + \phi_j + \phi_k = 1
$$

을 만족하는 $(\phi_i,\phi_j,\phi_k)$를 **barycentric coordinates**라고 한다.

이 값의 좋은 점은?
- vertex에 있는 어떤 attribute $a_i, a_j, a_k$도
$$
a(x) = \phi_i a_i + \phi_j a_j + \phi_k a_k
$$
처럼 **공짜로** 보간할 수 있다.
- 그리고 레스터화의 half-plane test 과정에서 “edge로부터의 거리”가 자연스럽게 계산되기 때문에 barycentric을 얻는 비용이 거의 없다.

>triangle 기반 파이프라인에서 barycentric은 사실상 표준 인터페이스.

---

## Perspective-incorrect vs Perspective-correct
![](../assets/images/Pasted%20image%2020260302002659.png)

투영된 2D 좌표로 Barycentric Coordinates를 계산하면,
사각형을 나눈 경계에서 보간 값의 (미분) 불연속이 생긴다.
(위 이미지에서 가운데 체커보드 무늬가 휘어지는 현상.)

### Perspective correct interpolation
문제를 해결하기 위해 보정 보간을 사용.

목표는 특정 정점에서 특정 속성 값을 보간하는 것.

깊이가 $Z$, attribute가 $\phi_i$일 때
$$
Z := \frac{1}{z},\quad
P := \frac{\phi_i}{z}
$$
를 먼저 계산한다.

표준(2D) barycentric coords로 $Z, P$를 보간한다.
$$
Z(x) = \sum \lambda_i Z_i,\quad
P(x) = \sum \lambda_i P_i
$$

마지막에 fragment에서 보간된 P를 보간된 Z로 나눠서 복원한다.
$$
\phi(x) = \frac{P(x)}{Z(x)}
$$

이렇게 하면 **3D에서 선형으로 변해야 하는 값**이 원근 투영 후에도 자연스럽게 보간된다.

---

## Texture Mapping
텍스처 매핑은 표면에 세부 디테일을 추가하는 가장 대표적인 방법.

### 텍스처 매핑의 활용 예시
- diffuse color(알베도), 로고/스크래치 등 “표면의 무늬”
- roughness/metallic 같은 “재질 파라미터”
- 여러 texture map을 레이어로 쌓아 복잡한 재질 표현

### Normal mapping & Displacement mapping
![](../assets/images/Pasted%20image%2020260302003711.png)
- **normal mapping**: 기하를 바꾸지 않고 법선만 바꿔 “울퉁불퉁해 보이게” 함  
  → 실루엣과 그림자는 그대로라 어색함.
- **displacement mapping**: 실제로 vertex를 변위시켜 기하를 바꿈  
  → 실루엣과 그림자 경계까지 자연스러워짐

### Precomputed lighting & shadows
![](../assets/images/Pasted%20image%2020260302003810.png)

- AO(ambient occlusion)처럼 미리 계산한 조명/그림자를 texture로 저장해 재사용
- environment map처럼 환경광 자체를 텍스처로 표현하기도 함

---

## Texture Coordinates
![](../assets/images/Pasted%20image%2020260302004110.png)

텍스처를 표면에 붙이려면 **표면의 한 점이 텍스처 이미지의 어디를 가리키는지**가 필요함.

이때 쓰는 게 **texture coordinates** $(u,v)$

보통 삼각형 정점의 텍스처 좌표를 선형 보간해서 정의

>실제로 (u,v)를 어떻게 만드는지는 UV unwrapping이라는 또 다른 주제다.  
>이번 강의에서는 “이미 (u,v)가 주어졌다”고 가정하고 샘플링/필터링을 다룬다.

![](../assets/images/Pasted%20image%2020260302004127.png)
택스처 좌표를 색과 연결하면 매핑을 시각화하는데 도움이 됨.

### Periodic / Tiling coordinates
큰 벽/바닥을 표현할 때 텍스처를 한 번만 깔면 해상도가 부족하다.  
그래서 작은 타일 텍스처를 여러 번 반복(tile)시키기 위해  
$(u,v)$가 0~1 범위를 넘어 주기적으로 반복되게 만들기도 한다.

![](../assets/images/Pasted%20image%2020260302004225.png)
![](../assets/images/Pasted%20image%2020260302004250.png) 

---

## Texture Sampling 101
![](../assets/images/Pasted%20image%2020260302212315.png)

**텍스처 매핑의 가장 단순한 알고리즘**

래스터화된 이미지의 각 픽셀에 대해:
1. 삼각형 위에서 (u, v)좌표 보간
2. 보간된 (u, v)에서 텍스처를 샘플(평가)
3. fragment 색을 샘플된 텍스처 값으로 설정

하지만 일반적으로 이렇게 간단하지 않음.

---

## Aliasing: 텍스처에서 더 심각해지는 이유
![](../assets/images/Pasted%20image%2020260302004518.png)

텍스처 샘플의 화면 공간 위치는 규칙적인 격자인데, 원근 투영 때문에 그 샘플이 텍스처 공간에서 차지하는 위치/간격은 불규칙해진다.

결과적으로
- 어떤 픽셀은 텍스처에서 아주 작은 영역만 커버하고
- 어떤 픽셀은 텍스처에서 엄청 큰 영역을 커버한다.

이 불균일성이 aliasing을 더 어렵게 만듬.

---

## Magnification vs. Minification
![](../assets/images/Pasted%20image%2020260302004618.png)
- **magnification(확대)**: 화면의 1픽셀이 텍스처의 **아주 작은 영역**만 덮음  
	- ex) 카메라가 물체에 매우 가까움
	- 픽셀 중심에서 값을 보간/샘플하면 됨

- **minification(축소)**: 화면의 1픽셀이 텍스처의 **큰 영역**을 덮음 
	- ex) 물체가 매우 멀리 있음
	- aliasing을 줄이려면 픽셀에 해당하는 영역의 평균 텍스처 값을 계산해야 함

---

## Bilinear Interpolation

확대(magnification) 상황에서 정수가 아닌 위치 $(u,v)$에서 텍스처 값을 어떻게 조회할까 ?

**선형 보간법(Bilinear Interpolation)** 을 통해서 색상 값들을 부드럽게 혼합할 수 있음.

과정을 단계적으로 살펴보자.

![](../assets/images/Pasted%20image%2020260302010049.png)
우선 픽셀을 찾아야 함. 텍셀(texture pixel) i,j값을 가져옴.
좌하단이기 떄문에 0.5씩 빼준다. (텍셀은 항상 픽셀의 중간에 중심이 있음)

![](../assets/images/Pasted%20image%2020260302010112.png)
(i, j) 값을 얻고 (u, v)에 가장 가까운 4개 texel 값을 $f_{00}, f_{10}, f_{01}, f_{11}$이라 한다.


![](../assets/images/Pasted%20image%2020260302010217.png)
(u, v)가 왼쪽 아래 모서리에서 얼마나 떨어져 있는지 나타내는 소수 값 s와 t.

이제 이 값들을 어떻게 블렌딩할 수 있을까 ?

![](../assets/images/Pasted%20image%2020260302010511.png)
먼저, 1차원 함수에서 했던 것처럼 수평 방향으로 선형 보간 수행.


![](../assets/images/Pasted%20image%2020260302010626.png)
두 행의 값을 다른 매개변수 t를 사용하여 혼합하여 이중 선형 보간(bilinear interporation)값을 얻음.
말 구대르 선형 보간 두 번 하는 거임.

---

## Prefiltering (minification)
![](../assets/images/Pasted%20image%2020260302010927.png)
텍스처가 매우 크고 객체에서 멀리 떨어져 있는 경우 위와 같은 아티팩트 발생.

![](../assets/images/Pasted%20image%2020260302010956.png)
이를 사전 필터링(Prefiltering)이라는 기술을 사용해서 해결.

**Prefiltering**
텍스처 앨리어싱은 화면의 한 픽셀이 텍스처의 많은 픽셀을 덮기 때문에 발생.
픽셀 중심에서 텍스처 값을 하나만 가져오면 앨리어싱이 생김.
평균 텍스처 값을 쓰면 좋겠지만 계산 비용이 큼.
그래서 결과적으로 평균을 미리 한 번 계산해 두고, 실행 시에 그 평균을 여러 번 조회.


---

## MIP Map
![](../assets/images/Pasted%20image%2020260302011410.png)
- 가능한 모든 스케일에서 Prefiltering된 이미지를 저장.
- 상위 레벨의 texel은 텍스처 공간의 한 영역 평균을 담음(downsampled)
- 나중에는 적절한 크기의 밉맵 레벨에서 픽셀 하나만 조회

### 저장 공간 오버헤드
면적이 $1 + 1/4 + 1/16 + \cdots = 4/3$ 배이므로  
mipmap을 만들면 원본 대비 약 **33%** 정도의 추가 저장 공간이 든다.

---

## Computing MIP Map Level
![](../assets/images/Pasted%20image%2020260302011706.png)

어떤 픽셀은 텍스처 공간에서 큰 영역을 덮고, 어떤 픽셀은 작은 영역을 덮는다.  
따라서 **픽셀마다 다른 mip level**을 쓰고 싶다.

**계산 방법**
이웃 픽셀 간의 텍스처 좌표 차이를 계산한다.

![](../assets/images/Pasted%20image%2020260302011808.png)
$$
\frac{du}{dx} \approx u_{10} - u_{00},\quad
\frac{dv}{dx} \approx v_{10} - v_{00}
$$
$$
\frac{du}{dy} \approx u_{01} - u_{00},\quad
\frac{dv}{dy} \approx v_{01} - v_{00}
$$

그리고 텍스처 공간에서의 길이를

$$
L_x^2 = \left(\frac{du}{dx}\right)^2 + \left(\frac{dv}{dx}\right)^2,\quad
L_y^2 = \left(\frac{du}{dy}\right)^2 + \left(\frac{dv}{dy}\right)^2
$$

$$
L = \sqrt{\max(L_x^2, L_y^2)},\quad
d = \log_2 L
$$

처럼 잡는다.  
여기서 $d$가 연속적인(실수) mip level이다.

---

## Problem with basic MIP mapping

![](../assets/images/Pasted%20image%2020260302011937.png)
만약 $d$를 가장 가까운 정수로 clamp해서 레벨을 고르면,  
레벨이 바뀌는 경계에서 텍스처가 갑자기 확 blur 되는 **jump artifact**가 생긴다.

**해결 방안**
밉맵 레벨을 정수로 클램프 하지 말고 원래의 연속적인 레벨 $d$를 사용하자!
-> 근데 밉맵은 유한한 레벨만 있는데 레벨 사이를 어떻게 보간할까?

---

## Trilinear Filtering (MIP Map Lookup)
우선 밉맵을 제외하고 생각해보자.

2D 데이터를 보간하기 위해 이중선형(Bilinear) 필터링을 쓴 것 처럼,
3D 데이터에는 삼중선형(Trilinear) 필터링을 쓸 수 있음.

![](../assets/images/Pasted%20image%2020260302012534.png)
점 (u, v, w)와 가장 가까운 8개 값이 주어졌을 때 선형 필터링을 반복하면 됨.

![](../assets/images/Pasted%20image%2020260302012522.png)
각각의 값에 대해서 선형 필터링을 반복하여 u, v, w방향의 가장 평균을 얻는다.

이제 다시 밉맵으로 돌아와서,
밉맵 보간도 본질적으로 동일하게 동작함.

![](../assets/images/Pasted%20image%2020260302012748.png)
- $d$를 정수로 만들지 말고,
- $\lfloor d \rfloor$ 레벨과 $\lfloor d \rfloor + 1$ 레벨을 둘 다 샘플한 뒤
- 그 둘을 다시 선형 보간한다.

절차:
1. 레벨 $\lfloor d \rfloor$에서 bilinear sample $c_0$
2. 레벨 $\lfloor d \rfloor+1$에서 bilinear sample $c_1$
3. $w = d - \lfloor d \rfloor$로
$$
c = (1-w)c_0 + w c_1
$$

>3차원 격자에서 trilinear처럼 보이지만,  
>실제로는 “두 레벨의 bilinear 결과를 한 번 더 섞는 것”이다.

연산/메모리 접근이 늘어나므로 GPU는 텍스처 샘플링을 위한 전용 하드웨어를 둔다.

---

## Anisotropic Filtering
![](../assets/images/Pasted%20image%2020260302012918.png)

원근이 강한 grazing angle에서는 픽셀이 텍스처 공간에서  
한 방향으로 매우 길게 늘어난 footprint를 갖는다.

이때 “정사각형 footprint”를 가정하는 isotropic(mipmap+trilinear) 필터는
- 한쪽 방향으로 과도하게 blur(= overblurring) 되는 문제가 생긴다.

이를 줄이기 위해 anisotropic filtering은
- 더 많은 샘플(여러 mipmap 샘플)을 결합해
- 늘어난 방향의 평균을 더 잘 근사한다.
(이렇게 하면 당연히 더 비쌈.)

---

## Texture Sampling Pipeline

1. 화면 샘플 $(x,y)$에서 barycentric으로 $(u,v)$ 계산
2. 이웃 샘플 차이로 $\frac{du}{dx},\frac{du}{dy},\frac{dv}{dx},\frac{dv}{dy}$ 근사
3. mip level $d$ 계산
4. $(u,v)\in[0,1]^2$를 텍스처 이미지 픽셀 좌표 $(U,V)$로 변환
5. 필요한 texel 주소 결정 (예: trilinear면 8개)
6. texel 로드
7. (U,V,d)에 맞춰 bilinear/trilinear 수행
8. (anisotropic이면 추가 작업)

>요점:  
>고품질 텍스처링은 픽셀 하나 조회보다 훨씬 많은 연산/대역폭이 필요하다.  
>그래서 GPU가 이 부분을 전용 하드웨어로 크게 최적화한다.

---

## 요약
- perspective projection은 3D primitive를 2D primitive로 바꿔 레스터화 가능하게 만든다.
  - view frustum은 clipping과 depth precision 관리(= z-fighting 완화)에 중요하다.
- 2D triangle 위에서의 attribute 보간은 barycentric coordinates가 사실상 표준이다.
  - 단, 3D에서 의미 있는 attribute는 perspective-correct interpolation이 필요하다.
- 텍스처 좌표 $(u,v)$는 2D 이미지를 3D 표면에 복사해 디테일을 추가한다.
- 텍스처는 aliasing이 특히 심해서 careful filtering이 필수다.
  - magnification: bilinear lookup으로 충분한 경우가 많다.
  - minification: 평균이 필요 → prefiltering → mipmap → trilinear
  - grazing angle: anisotropic filtering까지 고려
- 결론: 그래픽스는 항상 **퀄리티 vs 효율(연산/대역폭)** 의 균형을 잡는 일이다.