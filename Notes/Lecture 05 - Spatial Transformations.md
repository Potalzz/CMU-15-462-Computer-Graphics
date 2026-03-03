This post is a personal study note based on CMU 15-462 (Computer Graphics) 2020/fall lectures.
>All original lecture slides and videos are copyrighted by the instructors.

강의에 대한 정보와 자료는 아래 홈페이지에서 확인할 수 있습니다.
https://15462.courses.cs.cmu.edu/fall2020/home

본 포스팅은 강의 내용을 바탕으로 하되, 이해를 돕기 위해 별도의 자료 조사와 개념 정리를 덧붙여 작성했습니다. 따라서 원 강의 내용에 추가적으로 개인적인 학습 자료가 다수 포함되어 있습니다.

---

## 들어가며
이번 강의는 물체를 화면에 그리는 파이프라인 중에서 제일 앞단에 있는 공간에서 물체를 배치/변형하는 단계를 집중적으로 다룬다.

>**Spatial Transformation(공간 변환)**  
>각 점에 대해 새 위치를 할당하는 거의 모든 함수.  
>그중에서도 **선형 사상(linear map)** 으로 표현되는 변환(회전/스케일 등)에 집중할 것임.

---

## Review: Linear Maps
Q: $f:\mathbb{R}^n\to\mathbb{R}^n$ 가 **linear**하다는 건 무슨 뜻일까?

- **기하학적으로**: 직선을 직선으로 보내고, **원점을 보존**
- **대수적으로**: 벡터공간 연산(덧셈/스칼라곱)을 보존
  $$
  f(x+y)=f(x)+f(y),\quad f(ax)=af(x)
  
$$

![](../assets/images/Pasted%20image%2020260222185350.png)

### 왜 선형 변환이 중요한가?
- 적용이 빠름
- 선형시스템이라 풀기도 비교적 쉬움
- **선형 변환의 합성은 다시 선형** → 여러 변환을 곱해도 결국 **행렬 하나**로 묶을 수 있음
  GPU/API 입장에서는 이 통일된 표현이 매우 중요함

![](../assets/images/Pasted%20image%2020260222185435.png)

---

## Invariants of Transformation
>**어떤 변환인지**는 그 변환이 **무엇을 그대로 보존하는지(invariants)** 로 결정된다.

예)
- **linear**: 직선/원점 보존  
  $f(ax+y)=af(x)+f(y),\; f(0)=0$
- **translation**: 점들 사이 차이가 보존된다고 볼 수 있음
- **scaling**: 원점을 지나는 선(방향) 보존
- **rotation**: 원점/거리/방향성(orientation) 보존, $\det(f)>0$

---

## Rotation
회전은 다음 3가지 성질로 정의됨
1) 원점 고정  
2) 거리 보존  
3) orientation 보존

>첫 두 성질(원점+거리 보존)만으로도 회전은 선형임을 알 수 있다.

### 2D Rotation
![](../assets/images/Pasted%20image%2020260221223818.png)
$cos \theta$와 $sin \theta$는 단순히 원 위의 x좌표와 y좌표에 불과하다.

![](../assets/images/스크린샷%202026-02-21%20오후%2011.03.53.png)
결과적으로 2차원에서의 **반시계 방향으로 $\theta$만큼의 회전**은 위 행렬을 얻을 수 있음.

### 3D Rotation
3D에서 축 하나를 기준으로 회전할 땐,
- 회전 평면의 두 좌표에는 2D 회전을 적용하고
- 회전축 좌표는 그대로 둔다.

![](../assets/images/화면%20기록%202026-02-21%20오후%2011.16.3222.gif)

3차원에서도 2차원 행렬과 똑같은 형태의 행렬을 만들 수 있다.

### 회전 행렬의 역행렬 = 전치행렬
회전은 표준기저를 직교정규기저로 보내므로,
$$
R^TR=I \;\Rightarrow\; R^T=R^{-1}
$$

![](../assets/images/Pasted%20image%2020260221235547.png)
회전 행렬은 전치 행렬을 곱했을 때 항등 행렬이므로, 전치 행렬이 곧 역행렬이다.
전치 = 역행렬의 성질을 가지는 행렬을 직교행렬이라고 함.

전치가 역행렬이라는 것은
- 그 행렬이 길이와 각도를 보존하는 **순수 회전(또는 반사)** 변환이라는 뜻

회전 행렬은 원점의 위치와 원점으로 부터의 거리를 보존하기 때문에, 역행렬이 곧 전치 행렬이다.
(크기가 변하지 않아서 전치행렬을 곱하면 원래로 되돌릴 수 있음.)


---

## Reflections & Orthogonal Transformations
거리+원점을 보존하는 변환을 **orthogonal transformation**이라 하고,
$$
Q^TQ=I
$$
로 표현된다.

- **rotation**: orientation도 보존 → $\det(Q)>0$
- **reflection**: orientation을 뒤집음 → $\det(Q)<0$

“$Q^TQ=I$면 무조건 회전인가?”라는 질문에 대한 반례
![](../assets/images/Pasted%20image%2020260222190042.png)

---

## Scaling
>모든 벡터를 어떤 스칼라 $a$ 만큼 늘리거나 줄인다.  
>$f(u)=au$

- 방향 보존( $a\neq 0$ )
- 덧셈/스칼라곱 성질도 만족 → 선형 변환

### 행렬 표현
- uniform scaling: 대각선에 $a$
- axis-aligned nonuniform scaling(세 개의 축에 균일하지 않은 scaling): 대각선에 $a,b,c$

![](../assets/images/Pasted%20image%2020260222190128.png)

### Negative scaling
$a=-1$은 “축에 대한 반사(reflection)”의 연쇄로 볼 수 있다.
- 2D: 반사를 두 번 → orientation 보존
- 3D: 반사를 세 번 → orientation 뒤집힘

![](../assets/images/Pasted%20image%2020260222190225.png)

### 임의 축에 대한 Nonuniform scaling
축 정렬이 아니라 어떤 기울어진 축 기준으로 스케일하고 싶다면?

- 새 축으로 **회전 $R$**
- 그 축에서 diagonal scaling $D$
- 다시 되돌아오기 $R^T$

$$
f(x)=R^TDRx
$$

그리고 전체 변환 행렬은 대칭(symmetric) 형태
$$
A := R^TDR
$$

그럼 모든 대칭행렬은 어떤 축에 대한 nonuniform scaling인가?
→ 스펙트럴 정리에 따라 맞음.

**Spectral theorem(스펙트럴 정리)**
대칭행렬 $A=A^T$는

- **서로 직교하는(orthonormal) 고유벡터**들을 가지고
- **고유값은 실수**이며
- 행렬을 다음처럼 쓸 수 있다

$$A = R D R^T$$
- R: 회전
- D: 대각 행렬 (스케일)

---

## Shear
>어떤 방향 $u$로 “밀어내되”, 그 양은 다른 방향 $v$로의 거리(내적) $\langle v,x\rangle$에 비례.  

$$
f_{u,v}(x)=x+\langle v,x\rangle u
$$

그리고 행렬로는 $A_{u,v}=I+uv^T$.

---

## Composite Transformations
기본 변환을 행렬곱으로 합치면 합성 변환이 된다.

![](../assets/images/Pasted%20image%2020260222190636.png)

**순서가 중요함**
- 스케일 → 이동 vs 이동 → 스케일 결과가 다름.

원점 중심 회전만 하면, 물체가 원점 주위를 빙글빙글 돈다.

그래서 정석은
1) $-x$ 만큼 평행이동  
2) 회전  
3) $+x$ 만큼 평행이동

![](../assets/images/Pasted%20image%2020260222190856.png)

---

## Decomposition of Linear Transformations
주어진 선형 변환을 분해(Decomposition)하는 방법은 여러가지가 있다.
- **SVD** (신호처리)
- LU (선형시스템)
- **Polar decomposition (공간 변환에서 특히 유용)**

### Polar decomposition & SVD(Single Value Decomposition)
극 분해(Polar decomposition)는 아래의 형태로 행렬을 분해하는 정리.
$$
A = QP
$$
- $Q$: 길이 보존 → 회진/반사
- $P$: 방향별로 음수 없이 늘이거나 줄이는 변환

위의 수식이 기하학적으로 무슨 의미인지 살펴보자.

어떤 물체가 변형될 때 선형 변환은 총 2단계를 거친다.

**1. P (stretch 단계)**
- 특정 방향으로 늘어나고
    
- 다른 방향으로 줄어듦
    
- 하지만 뒤집히지는 않음 (음수 스케일 없음)
    
>찌그러뜨리기 단계

**2. Q (rotation 단계)**
- 전체가 회전하거나
    
- 반사됨

>방향 바꾸기 단계

결과적으로 모든 선형변환은 **stretch + rotation** 구조를 가진다.


그리고 $P$는 대칭 행렬이므로 위에서 정리한 스펙트럴 정리(spectral theory)에 따라 더 쪼갤 수 있다.
$$
\begin{align}
&P = (\text{rotate})(\text{axix-aligned scaling})(\text{rotate back}) \\
&P = VDV^T
\end{align}
$$
결국 $UDV^T$ 형태의 SVD로 이어진다.

![](../assets/images/Pasted%20image%2020260222191039.png)

결과적으로 모든 선형 변환은 **회전과 스케일의 조합**이며,
이를 표현하는 두 가지 방식이
Polar decomposition과 SVD(Sigle Value Decomposition)이다.

### 왜 이런 분해가 그래픽스에 유용할까?
변환 $A_0\to A_1$ 사이를 애니메이션으로 자연스럽게 이어주고 싶다.

- 나이브한 방법: 행렬 자체를 선형보간  
  $$
  A(t)=(1-t)A_0+tA_1
  
$$
  시간이 1에 가까워 질 수록 $A_0\to A_1$으로 변함.
  시작/끝은 맞지만, 중간 모양이 끔찍해질 수 있다.

- 더 나은 방법: polar decomposition으로 나눠서 보간  
  $A_0=Q_0P_0,\; A_1=Q_1P_1$에서  
  $P(t)$는 선형보간하고, $Q(t)$도 회전 성분 기준으로 다룬 뒤 $A(t)=Q(t)P(t)$.

![](../assets/images/Pasted%20image%2020260222191242.png)


![](../assets/images/화면%20기록%202026-02-22%20오후%207.14.01.gif)
캐릭터 스키닝에서도 나이브한 선형 보간은 **candy-wrapper artifact**(캐릭터의 관절 부분이 찝히는 효과)를 만들 수 있음.

이를 피하기 위해 다양한 변환 블렌딩 방법이 연구되어있음.

---

## Translation
이동(translation)은 원점이 변하기 때문에 선형 변환이 아니라 **아핀 변환(affine map)** 이다
회전, 스케일, shear는 전부 행렬 곱 하나로 처리가 가능하지만,
translation만 따로 벡터를 더해야 한다면 GPU 파이프라인이 복잡해짐.

4차원으로 가면 이동이 선형 변환으로 바뀔 수 있다.
그래서 나온게 **동차 좌표계(Homogeneous Coordinates)**

---

## Homogeneous Coordinates (동차좌표)
동차좌표는 원래 원근법 연구에서 출발했고, **직선에 좌표를 부여**하는 방법.

**기본 아이디어(2D를 3D로 올려다보기)**
- 원점을 지나지 않는 어떤 3D 평면(예: $z=1$)을 잡자.
- 원점을 지나는 직선 하나는 그 평면을 딱 한 번 뚫고 지나가며, 그 교점이 2D 점을 대표한다.
- 이 이야기는 pinhole camera의 같은 광선 위의 점들은 같은 픽셀로 가는 것과 닮았다.

2D 점 $p=(x,y)$는 3D에서
$$
\hat p=(cx,cy,c),\quad c\neq 0
$$
로 표현되고, 스칼라 배만큼 달라도 같은 점(같은 직선)을 의미한다.

![](../assets/images/Pasted%20image%2020260222191719.png)


![](../assets/images/화면%20기록%202026-02-22%20오후%204.46.39.gif)
2D에서 translation은 3D에서의 shear와 같다.

우리는 2D에서 $p = (p_1, p_2)$를
$\hat p = (cp_1, cp_2, c)$ 로 쓴다.

이 상태에서 translation $(p_1, p_2) \to (p_1 + u_1, p_2 + u_2)$ 를 해보면
$(cp_1, cp_2, c) \to (cp_1 + cu_1, cp_2 + cu_2, c)$가 나온다.

여기서 중요한 점은 c 방향으로 얼마나 떨어져 있는지에 비례해서 x,y가 이동한다는 것.

이건 **shear와 동일한 형태**이고, shear는 선형 변환이다.


그래서 translation이

$$\begin{bmatrix} 1 & 0 & u_1 \\ 0 & 1 & u_2 \\ 0 & 0 & 1 \end{bmatrix}​$$​​
라는 **하나의 행렬 곱**으로 표현된다.
결과적으로 2D의 affine 변환이 3D homogeneous 공간에서는 linear 변환이 된다.

![](../assets/images/Pasted%20image%2020260222191855.png)

2D 회전/스케일도 homogeneous space에서 회전축 회전/축별 스케일로 표현이 가능.

### 3D에서 동차좌표계
3D 점 $(x,y,z)$에 homogeneous coordinate를 하나 붙여 $(x,y,z,1)$로 만들면,
기존 3D linear 변환은 4x4에서 block으로 들어가고, translation은 shear로 들어간다.

![](../assets/images/Pasted%20image%2020260222192017.png)

---

## Points vs Vectors
동차좌표에서
- 점(point): $c=1$ (일반적으로 basepoint가 있다)
- 벡터(vector): $c=0$ (방향/크기만 있고 basepoint가 없다)

이 구분이 왜 중요하냐면 법선(normal)을 변환할 때 translation을 먹이면 안된다.
삼각형을 회전/이동했을 때 normal은 그냥 회전만 해야지, translation이 섞이면 이상해짐.

**해결방안**
>normal 같은 벡터는 homogeneous coordinate를 **0**으로 둔다.  
>그러면 translation 성분이 무시되고, 다시 직교성이 맞는다.

![](../assets/images/Pasted%20image%2020260222192227.png)

---

## Perspective Projection in homogeneous coordinates
pinhole camera에서 핵심은 $z$로 나누기(divide by z)
![](../assets/images/Pasted%20image%2020260222192258.png)

동차좌표에서는 $z$를 homogeneous coordinate로 복사하는 행렬을 만든 다음,
동차 나눗셈을 하면
$$
(x,y,z)\mapsto (x/z,\;y/z)
$$
가 된다.

![](../assets/images/Pasted%20image%2020260222192327.png)

---

## Screen Transformation (view plane → pixel)
원근 투영까지 끝나면, 이제 viewing plane 위의 좌표를 **W×H 픽셀 좌표**로 바꿔야 한다.
- E.g, $z=1$ 평면의 $[0,1]\times[0,1]$을 이미지로 매핑
- 주의: 이미지 좌표계는 보통 **y가 아래로 증가**함

![](../assets/images/Pasted%20image%2020260222192424.png)

---

## Scene Graph & Instancing
복잡한 장면에서는 **큐브 하나**가 아니라 **큐브 여러 개 + 계층 구조**가 된다.
이때 scene graph가 변환을 정리해줌.

- directed graph에 **상대 변환(relative transform)**을 저장
- 어떤 노드에 적용되는 변환은 루트부터 그 노드까지의 행렬곱(합성)
- 중복 곱셈을 줄이려고 transform을 **스택**으로 관리

![](../assets/images/Pasted%20image%2020260222192528.png)

### Instancing
같은 오브젝트를 잔뜩 찍어내야 하면(풀/군중/나무),
지오메트리를 복사하지 말고 scene graph에 **포인터 노드**로 여러 번 참조하자. 각 참조(간선)에 다른 변환만 달면 된다.

![](../assets/images/Pasted%20image%2020260222192608.png)

---

## Drawing a Cube Creature
전체적인 과정 정리
1) scene graph로 여러 큐브에 3D 변환 적용  
2) 카메라 위치 변환  
3) perspective projection  
4) image coordinates 변환 후 rasterize![](../assets/images/Pasted%20image%2020260222192714.png)

---

## 요약
**기본 선형 변환**
- scaling / rotation / reflection / shear

**복합 변환**
- 기본 변환을 합성해서 만든다.
- homogeneous coordinates로 쓰면 결국 **하나의 4x4 행렬**로 줄어든다.
- **순서(order)가 중요**하다.

**분해(decomposition)**
- polar, SVD 등으로 의미 있는 성분(회전 vs 스케일)을 분리할 수 있고, 애니메이션/보간에서 특히 유용하다.

**비선형처럼 보이는 것들도 동차좌표계에서 선형이 된다**
- translation, perspective projection