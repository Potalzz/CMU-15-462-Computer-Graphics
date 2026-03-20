>This post is a personal study note based on CMU 15-462 (Computer Graphics) 2020/fall lectures.
>All original lecture slides and videos are copyrighted by the instructors.

강의에 대한 정보와 자료는 아래 홈페이지에서 확인할 수 있습니다.
https://15462.courses.cs.cmu.edu/fall2020/home

본 포스팅은 강의 내용을 바탕으로 하되, 이해를 돕기 위해 별도의 자료 조사와 개념 정리를 덧붙여 작성했습니다. 따라서 원 강의 내용에 추가적으로 개인적인 학습 자료가 다수 포함되어 있습니다.

---

## Core Idea

큰 흐름은 최적화, ODE, PDE가 서로 이어진다는 점.

- 최적화에서 자주 쓰는 gradient descent도 시간에 따라 상태가 바뀌는 과정으로 볼 수 있다
- ODE는 시간 미분만 다룬다
- PDE는 시간 미분에 더해 공간 미분까지 함께 다룬다
- 그래서 물, 연기, 천, 머리카락, 탄성체처럼 공간 전체가 함께 움직이는 현상을 표현하기 좋다

간단히 말하면 ODE는 한 점의 움직임에 가깝고, PDE는 공간에 퍼진 상태의 변화에 가깝다.  
![](../assets/images/Pasted%20image%2020260318194022.png)

## Why Graphics Uses PDEs

그래픽스에서 PDE가 중요한 이유는 실제 세계의 많은 현상이 주변과의 상호작용으로 결정되기 때문.

- 물결은 주변 높이 차이에 따라 움직인다
- 열은 주변으로 퍼진다
- 연기와 유체는 흐름과 회전을 만든다
- 천과 머리카락은 힘을 받으면 변형되고 다시 돌아가려 한다

그래서 물리 기반 애니메이션에서는 PDE가 핵심 도구.  

실제 다양한 응용 존재.
- 액체 시뮬레이션
- 연기 시뮬레이션
- 천 시뮬레이션
- 탄성체, 머리카락, 파손, 점탄성, 눈 시뮬레이션

## What Is a PDE

PDE는 시간과 공간에 대한 함수 $u(t,x)$를 푸는 문제.

ODE와 PDE를 비교하면 감이 잡힌다.

- ODE 예시: 공중으로 던진 물체의 위치 $x(t)$
- PDE 예시: 연못 표면 높이 $h(t,x,y)$

예를 들면

$$
\frac{d^2 x(t)}{dt^2} = g
$$

는 시간에 따른 물체의 가속도를 말하고,

$$
\frac{\partial^2 h(t,x,y)}{\partial t^2} = \Delta h(t,x,y)
$$

는 물결 높이의 시간 변화가 공간에서의 굽힘과 연결된다는 뜻.

핵심은 식이 해를 직접 주지 않는다는 점.  
어떤 관계를 만족해야 하는지만 알려준다.  
그래서 실제 애니메이션을 만들려면 수치적으로 풀어야 한다.

조금 더 일반적으로는 시간 미분, 공간 미분, 더 높은 차수의 미분이 섞여 들어간다.  
예시로 버거스 방정식은
![](../assets/images/Pasted%20image%2020260318210744.png)

형태.  
왼쪽은 이동과 비선형 효과, 오른쪽은 퍼짐을 나타내는 식으로 볼 수 있다.  

## PDE Anatomy

PDE를 볼 때 먼저 확인할 두 가지가 있다.

- 선형인가, 비선형인가
- 몇 차 미분까지 들어가는가

### Linear vs. Nonlinear

- 선형 PDE는 함수와 미분항이 단순히 더해지는 형태
- 비선형 PDE는 $u \dfrac{\partial u}{\partial x}$처럼 함수끼리 곱해지거나 더 복잡하게 얽힌 형태

보통 비선형이 더 어렵다.

### Order

- 시간에 대한 1차 미분이면 시간 1차
- 공간에 대한 2차 미분까지 있으면 공간 2차

예를 들어 Burgers equation은 시간 1차, 공간 2차.  
Wave equation은 시간 2차, 공간 2차.  
대체로 고차일수록, 비선형일수록 어렵다.  

## Model Equations

강의에서는 세 가지 모델 방정식을 중심으로 설명한다.

### Laplace Equation
![](../assets/images/Pasted%20image%2020260318212615.png)

$$
\Delta u = 0
$$

경계에 주어진 값을 가장 부드럽게 이어 주는 식.  
직관적으로는 각 값이 주변 이웃의 평균과 같아지는 상태.

주변 값들을 계속 평균내서 값을 구하므로, 에러가 점점 줄어들어 에러에 강함.

### Heat Equation
![](../assets/images/Pasted%20image%2020260318212642.png)

$$
\frac{\partial u}{\partial t} = \Delta u
$$
열이 시간이 지나며 퍼지는 과정을 표현한다.  
확산, 감쇠, 점성 같은 현상을 모델링할 때도 자주 등장.

오래 지나 더 이상 값이 바뀌지 않으면, 그 최종 상태가 Laplace equation의 해와 연결된다.

### Wave Equation
![](../assets/images/Pasted%20image%2020260318212705.png)
$$
\frac{\partial^2 u}{\partial t^2} = \Delta u
$$

파동이 퍼져 나가는 과정을 표현한다.  
연못에 돌을 던졌을 때 생기는 물결 같은 상황.

이 식은 heat equation보다 풀기 더 까다롭다.  
초기 오차가 오래 남고 멀리 전파되기 때문.  

## Numerical View

대부분의 PDE는 손으로 정확한 해를 쓰기 어렵다.  
그래서 수치적으로 푼다.

기본 흐름은 아래와 같다.

- 무엇을 미지수로 둘지 정한다
- 공간 미분을 어떻게 근사할지 정한다
- 시간 미분을 어떻게 근사할지 정한다
- 그 결과로 update rule을 만든다
- 이 규칙을 반복해서 애니메이션을 만든다

ODE에서 했던 forward Euler, backward Euler 같은 시간 적분 아이디어가 그대로 이어진다.  
여기에 공간 이산화가 추가될 뿐.

강의의 핵심 메시지 하나.  
PDE 코드는 수식만큼 복잡하지 않음.
격자 위를 돌면서 이웃 값을 모아 계산하는 형태가 매우 자주 나온다.  

## Space Representation

공간을 나누는 방식에는 크게 두 가지가 있다.

### Lagrangian
![](../assets/images/Pasted%20image%2020260318213207.png)
움직이는 입자나 점을 따라가는 방식.

- 입자의 위치와 속도를 추적
- 입자 기반이라 개념이 직관적
- 격자 크기에 갇히지 않음
- 대신 이웃을 찾거나 입자 분포를 잘 유지하는 일이 어려울 수 있음

### Eulerian
![](../assets/images/Pasted%20image%2020260318213215.png)
고정된 격자 위에서 값을 기록하는 방식.

- 격자 칸마다 속도나 유량을 기록
- 계산이 규칙적이라 빠르다
- 미분 근사가 비교적 쉽다
- 대신 격자 해상도에 묶이고, 수치적 번짐이 생길 수 있음

실전에서는 둘을 섞기도 한다.
PIC, FLIP, particle level set, material point method 같은 방식이 그 예.  

## Which Quantity to Solve

같은 물리 현상도 무엇을 변수로 잡느냐에 따라 계산이 달라진다.

예를 들어 비압축성 유체에서는

- 속도를 풀 수도 있고
- 회전량을 풀 수도 있다

속도는 각 지점이 얼마나 빠르게 움직이는지에 가깝고,  
회전량은 얼마나 회전하는지에 가깝다.

수학적으로는 연결되어 있어도 계산 특성은 꽤 다를 수 있다.  
문제에 맞는 표현을 고르는 것이 중요.  

## Laplacian

이 강의의 중심 연산자.  
Laplace, heat, wave equation 모두 라플라시안 연산자를 사용함.

라플라시안 $\Delta u$는 여러 방식으로 이해할 수 있다.

- 각 방향의 2차 미분 합
- 기울기 변화를 다시 모아 본 값
- 주변 평균에서 얼마나 벗어나는지

이 셋이 같은 대상을 다른 관점에서 설명한다는 점이 중요하다.

특히 직관적으로는  
현재 값이 주변 이웃 평균보다 얼마나 튀어 있는지 재는 도구라고 보면 편하다.

- 주변과 거의 같으면 라플라시안이 작다
- 주변보다 크게 튀면 라플라시안이 크다

그래서 파동, 확산, 매끄러움과 모두 연결된다.  

## Finite Differences

정규 격자에서는 미분을 차분으로 바꿔 근사할 수 있다.

### First Derivative
![](../assets/images/Pasted%20image%2020260318213754.png)
$$
u'(x_i) \approx \frac{u_{i+1} - u_i}{h}
$$

가장 가까운 두 점의 차이를 이용하는 방식.

### Second Derivative
![](../assets/images/Pasted%20image%2020260318213813.png)

**1차 미분**이 값이 커지느냐/작아지느냐 였다면,
**2차 미분**은 기울기가 커지느냐/작아지느냐를 의미한다.

이계미분 값은 오른쪽 기울기 - 왼쪽 기울기 이므로,
위로 솟아오른 봉우리에서의 이차 미분값은 음수가 나오고,
아래로 볼록한 봉우리에서의 이차 미분값은 양수가 나온다.

그래픽스에서는 연속 함수를 점들로 쪼개서 계산하기 때문에,
이계 미분을 직접 구하지 못하고 근사한다.

위 식에서 가운데 점 $u_i$, 양 옆 점 $u_{i-1}, u_{i+1}$가 있으면,
왼쪽 기울기:
$$
\dfrac{u_i - u_{i-1}}{\nabla x}
$$

오른쪽 기울기:
$$
\dfrac{u_{i+1} - u_i}{\nabla x}
$$

이 둘의 차이를 다시 $\nabla x$로 나누면,
$$u''(x_i)\approx \dfrac{\dfrac{u_{i+1}-u_i}{\Delta x}-\dfrac{u_i-u_{i-1}}{\Delta x}}{\Delta x}
= \dfrac{u_{i-1}-2u_i+u_{i+1}}{\Delta x^2}$$
이게 central diffrence이다. 최종적으로 맨 위에서 제시한 수식이 나온다.

즉, 이계미분의 이산 근사는 **현재 값이 양옆 평균과 얼마나 다른가**를 보는 꼴로도 해석된다.

### Grid Laplacian
![](../assets/images/Pasted%20image%2020260318213842.png)

2차원 격자에서는

$$
\Delta u_{i,j} \approx
\frac{
4u_{i,j} - u_{i+1,j} - u_{i-1,j} - u_{i,j+1} - u_{i,j-1}
}{h^2}
$$

형태가 된다.

자기 자신을 네 번 곱하고, 즉 위, 아래, 왼쪽, 오른쪽 이웃을 빼는 구조.  
이웃 평균과의 차이를 직접 계산하는 형태.

삼각형 메쉬에서는 같은 생각을 cotan weight 형태로 일반화할 수 있다.  
격자가 아니어도 비슷한 PDE 아이디어를 계속 쓸 수 있다는 점이 중요하다.  

## Solving Laplace Equation
![](../assets/images/Pasted%20image%2020260318214110.png)
Laplace equation을 격자에서 풀면 각 칸의 값이 이웃 평균과 같아야 한다.

$$
u_{i,j}
=
\frac{
u_{i-1,j} + u_{i+1,j} + u_{i,j-1} + u_{i,j+1}
}{4}
$$

이런 함수를 harmonic function이라고도 부른다.

### Two Ways to Solve

- 반복 평균내기
- 큰 선형방정식으로 바꿔 풀기

반복 평균내기는 개념이 쉽다.  
모든 칸을 계속 평균으로 바꾸면 결국 수렴한다.  
하지만 큰 격자에서는 많이 느리다.

그래서 보통은 각 칸에 번호를 붙여 큰 선형시스템 $Ax=b$로 만든 뒤,  
0이 대부분인 큰 선형방정식을 잘 푸는 도구를 사용한다.

단, 경계 조건 없이 $\Delta u = 0$만 두면 재미있는 해가 잘 나오지 않는다.  
상수 함수도 해가 되기 때문.  
무엇을 경계에 둘지 정해 주어야 한다.  

## Boundary Conditions
라플라스 방정식을 사용하기 위해서는, 주변 값들을 평균내어야 하는데 경계를 벗어나 버리면 안됨.
경계값을 어떻게 정의해야 할까?

경계 조건은 PDE의 해를 결정하는 데 매우 중요함.

![](../assets/images/Pasted%20image%2020260318214640.png)
경계 근처에서 값을 평균내는데 어떤 값을 써야할까 ?

PDE에는 다양한 경계 조건이 있지만, 두 가지 기본 경계 조건이 있음.

1. **Dirichlet**: 경계 데이터는 항상 고정된 값
2. **Neumann**: 물음표 노드로 이동할 때 값의 변화량 즉, 미분값을 지정.

### Dirichlet
![](../assets/images/Pasted%20image%2020260318214251.png)
경계에서 함수값 자체를 지정한다.

예시:

$$
\phi(0)=a,\quad \phi(1)=b
$$

경계의 온도, 높이, 색 등을 직접 주는 느낌.

### Neumann
![](../assets/images/Pasted%20image%2020260318214303.png)
경계에서 미분값을 지정한다.

예시:

$$
\phi'(0)=u,\quad \phi'(1)=v
$$

경계에서 얼마나 들어오고 나가는지를 정하는 느낌.

### Why Boundary Conditions Matter

경계 조건은 아무렇게나 주면 안 된다.  
어떤 PDE는 특정 조건에서만 해가 존재한다.

- 1차원 Laplace equation $\phi''=0$의 해는 직선 $\phi(x)=cx+d$
- Dirichlet 값 두 개는 언제나 직선 하나로 이을 수 있다
- 하지만 Neumann 값 두 개는 항상 가능한 것이 아님
- 직선의 기울기는 하나뿐이기 때문

2차원에서도 비슷한 제약이 있다.  
특히 Neumann 조건은 전체 유입과 유출의 합이 맞아야 한다.

Laplace equation에서

$$
\int_{\partial \Omega} n \cdot \nabla \phi \, ds
=
\int_{\Omega} \Delta \phi \, dA
=
0
$$

이어야 하므로, 경계 전체의 들어오고 나가는 값의 총량이 0이어야 해가 존재할 수 있다.

- 계산 결과를 바로 믿지 않기
- $Ax-b$가 충분히 작은지 확인하기

경계 조건이 잘못되면 라이브러리가 실패할 수 있으니 주의.

## Heat Equation

Heat equation은 시간에 따라 값이 퍼지는 문제.

$$
\frac{\partial u}{\partial t} = \Delta u
$$

forward Euler를 쓰면

$$
u^{k+1} = u^k + \Delta u^k
$$

Grid에서는

$$
u_{i,j}^{k+1} = u_{i,j}^k + \dfrac{\tau}{h^2}\left(4u_{i,j}^k - u_{i+1,j}^k - u_{i-1,j}^k - u_{i,j+1}^k - u_{i,j-1}^k\right)
$$

가 된다.

해석은 간단하다.

- 현재 값이 주변보다 크면 내려간다
- 현재 값이 주변보다 작으면 올라간다
- 결국 주변과 비슷해지며 매끈해진다

그래서 heat equation은 smoothing, diffusion, damping의 기본 모델로 자주 쓰인다.  
오랜 시간이 지나 더 이상 안 바뀌는 상태에 도달하면 Laplace equation과 연결된다.  

## Wave Equation

Wave equation은 퍼짐보다 진동과 전파에 가깝다.

$$
\frac{\partial^2 u}{\partial t^2} = \Delta u
$$

이 식을 푸는 방법은 크게 두 가지.

### Split into First Order Equations

새 변수 $v$를 속도로 두면

$$
\frac{\partial u}{\partial t} = v,\quad
\frac{\partial v}{\partial t} = \Delta u
$$

로 바꿀 수 있다.

즉 높이와 속도를 따로 업데이트하는 방식.  
강의 초반 예제 코드가 이 아이디어.

### Centered Difference in Time

시간에 대한 2차 차분을 직접 쓰면

$$
\frac{u^{k+1} - 2u^k + u^{k-1}}{\tau^2}
\approx
\Delta u^k
$$

로 둘 수 있다.

이 방식은 이전 두 시점의 값이 필요하다.  
반면 첫 번째 방식은 초기 속도가 필요하다.

핵심 차이는 heat equation처럼 단순히 퍼지지 않는다는 점.  
오차도 함께 전파되기 때문에 더 조심해서 풀어야 한다.  

## Implementation Pattern
![](../assets/images/화면%20기록%202026-03-19%20오전%2012.12.41.gif)
강의의 물결 예제 코드는 매우 전형적인 패턴을 보여 준다.

- 격자에 높이 $u$와 속도 $v$를 저장
- 매 프레임마다 이웃 값을 모아 라플라시안을 근사
- 그 값으로 속도를 갱신
- 속도로 높이를 갱신
- 필요하면 damping을 곱해 에너지를 조금씩 줄임

즉 PDE 시뮬레이션의 기본 모양은  
**이웃을 본다 → 변화량을 만든다 → 시간을 한 칸 전진시킨다**  
의 반복.

## 요약

- PDE는 시간과 공간이 함께 변하는 현상을 다루는 기본 언어
- PDE의 세 가지 모델 방정식: Laplace, heat, wave equation
- 라플라시안은 주변 평균과의 차이를 재는 연산자로 이해하면 편하다
- 수치해석의 핵심은 공간 이산화와 시간 이산화
- Grid에서는 finite difference가 가장 직관적
- 경계 조건은 해의 존재와 품질을 좌우한다
- 같은 물리 현상도 어떤 양을 미지수로 둘지에 따라 계산 성질이 달라진다
- 실전에서는 속도와 안정성을 위해 적절한 표현과 계산 도구 선택이 중요

그래픽스에서 왜 PDE가 중요한지와 가장 기본적인 수치해법의 뼈대를 잡아 줌.
이 뼈대를 이해하면 이후의 유체, 천, 탄성체 시뮬레이션도 훨씬 읽기 쉬워진다.
