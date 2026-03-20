>This post is a personal study note based on CMU 15-462 (Computer Graphics) 2020/fall lectures.
>All original lecture slides and videos are copyrighted by the instructors.

강의에 대한 정보와 자료는 아래 홈페이지에서 확인할 수 있습니다.
https://15462.courses.cs.cmu.edu/fall2020/home

본 포스팅은 강의 내용을 바탕으로 하되, 이해를 돕기 위해 별도의 자료 조사와 개념 정리를 덧붙여 작성했습니다. 따라서 원 강의 내용에 추가적으로 개인적인 학습 자료가 다수 포함되어 있습니다.

---

### Generalized Coordinates
![](../assets/images/Pasted%20image%2020260316233743.png)

일반화 좌표는 시스템의 자유도를 하나의 큰 벡터로 모아 표현한 것이다.
입자가 여러 개라면 각 위치를 차례로 쌓아 $q=(x_0, x_1, \dots, x_n)$처럼 쓸 수 있다.
진자처럼 각도 하나만으로도 표현할 수 있다.
컴퓨터 솔버는 긴 벡터를 다루는 데 익숙하므로 이런 표현이 자연스럽다.

### Ordinary Differential Equations

많은 동적 시스템은 ODE(상미분방정식)로 표현한다.
시간에 따라 상태가 어떻게 변하는지를 어떤 함수로 나타내는 형태다.

1차 ODE는 보통 $\dot q = f(q,t)$ 꼴이다.
2차 운동 방정식도 속도를 따로 두면 1차 시스템 두 개로 바꿀 수 있다.
예를 들어 Newton 식은 $\dot q = v$, $\dot v = M^{-1}F(q,v,t)$처럼 분리할 수 있다.
ordinary라는 말은 공간에 대한 미분이 아니라 시간에 대한 미분만 다룬다는 뜻이다.

### Lagrangian Mechanics

힘을 직접 쓰기보다 에너지로 표현하는 방법이다.

먼저 운동 에너지 $K$와 위치 에너지 $U$를 쓴다.
그다음 라그랑지안을 $$\mathcal L = K - U$$로 둔다.
이후 오일러-라그랑주 방정식 $$\frac{d}{dt}\frac{\partial \mathcal L}{\partial \dot q} = \frac{\partial \mathcal L}{\partial q}$$로 운동 방정식을 얻는다.
즉 라그랑지안 $\mathcal L$ 자리에 $K-U$를 넣어주기만 해도 운동 방정식이 나오는 간결한 구조다.

![](../assets/images/Pasted%20image%2020260316234555.png)

이 방식의 장점은 힘보다 에너지로 쓰는 편이 쉬운 경우가 많고, 일반화 좌표와도 잘 맞는다는 점이다.
또한 symplectic 계열 적분기를 이해하는 데도 이어진다.

### Lagrangian Mechanics - Example
![](../assets/images/Pasted%20image%2020260316234504.png)

진자의 일반화 좌표는 각도 $q=\theta$ 하나다.
줄에 고정되어 있어 원의 둘레 위에서만 움직일 수 있기 때문이다.
운동 에너지는 $K=\dfrac12 mL^2\dot\theta^2$, 위치 에너지는 $U=-mgL\cos\theta$다.

여기서 $\mathcal L = K-U$를 만들고 오일러-라그랑주 방정식을 적용하면 $\ddot\theta = -\dfrac{g}{L}\sin\theta$가 된다.
작은 각도에서는 $\sin\theta \approx \theta$이므로 $\ddot\theta \approx -\dfrac{g}{L}\theta$로 볼 수 있다.

### Why Numerical Simulation
![](../assets/images/Pasted%20image%2020260316234919.png)

간단해 보이는 시스템도 해가 금방 복잡해진다.
이중 진자는 작은 초기값 차이가 큰 결과 차이로 이어지는 chaotic system이다.
즉 초기 상태에 매우 민감하다는 뜻이다.

$n$-body 문제도 $n \ge 3$부터는 보통 closed-form 해를 기대하기 어렵다.

그래서 애니메이션에서는 해를 식으로 완전히 써내기보다, 타임스텝을 따라 수치적으로 전진시키는 방법이 핵심이다.

### Particle Systems

많은 현상은 입자 집합으로 모델링할 수 있다.
각 입자에 간단한 힘 규칙을 주고, 전체를 함께 적분한다.

flocking에서는 이웃의 중심 쪽으로 끌리는 힘, 너무 가까울 때 밀어내는 힘, 평균 진행 방향에 맞추는 alignment를 사용한다.
이런 로컬 규칙만으로도 전체적으로 복잡한 집단 움직임이 나타난다.

장점은 이해와 구현이 쉽고 스케일 조절도 쉽다는 점이다.
단, 유체처럼 입자 수가 매우 커지면 kd-tree, BVH 같은 가속 구조가 필요해진다.
이때는 continuum model, 즉 연속적인 장으로 다루는 방식이 더 적합할 수도 있다.

### Mass-Spring Models
![](../assets/images/Pasted%20image%2020260316235043.png)

mass-spring system은 입자 사이를 스프링으로 연결한 모델이다.
두 점 $x_1, x_2$를 자연 길이 $L_0$인 스프링으로 연결하면 위치 에너지는 $$U(x_1,x_2)=\frac{k}{2}(\|x_1-x_2\|-L_0)^2$$가 된다.
스프링 여러 개를 엮으면 천, 연성 물체, 머리카락, 캐릭터 의상 같은 형태를 표현할 수 있다.
직관적이고 구현이 쉬워서 그래픽스와 게임에서 매우 흔하게 쓰인다.
다만 재질의 거동을 더 정확하게 담으려면 continuum model이 더 나은 경우도 많다.

### Time Integration
![](../assets/images/Pasted%20image%2020260320111841.png)
수치 적분의 기본 생각은 미분을 차분으로 바꾸는 것이다.
미분(연속적) -> 차분(이산적)

연속 함수 $q(t)$ 대신 시간 샘플 $q_k$를 다루고, 타임스텝 $\tau$만큼 뒤의 상태 $q_{k+1}$를 구한다.

중요한 질문은 속도나 미분 함수 $f$를 어느 시점에서 평가하느냐는 것이다.
현재 상태인지, 다음 상태인지, 아니면 둘을 섞는지에 따라 성질이 크게 달라진다.

### Forward Euler
![](../assets/images/Pasted%20image%2020260320112227.png)

> 현재 상태에서 속도를 평가한다.

가장 단순한 명시적 적분기.
`*적분기: 현재 상태를 보고, 아주 짧은 시간 뒤의 다음 상태를 계산하는 규칙`

식은 $$q_{k+1}=q_k+\tau f(q_k)$$
현재 상태에서 접선 방향으로 조금 이동한다고 해석할 수 있어 구현이 매우 쉽다.
대신 안정성이 약하다.
진자 예시에서는 실제보다 에너지가 커지면서 진폭이 점점 불어날 수 있다.

선형 감쇠식 $\dot u=-au$에 적용하면 $u_{k+1}=(1-\tau a)u_k$가 된다.
안정하려면 $|1-\tau a|<1$, 즉 $\tau<2/a$가 필요하다.
실제로 a가 크면(stiff system)에서는 아주 작은 타입 스텝이 필요하다.
(e.g 매우 강한 스프링처럼 빠른 변화가 섞인 시스템)

### Backward Euler
![](../assets/images/Pasted%20image%2020260320113238.png)

암시적 적분기의 대표적인 예.
식은 $$q_{k+1}=q_k+\tau f(q_{k+1})$$
다음 상태가 식 안에 다시 들어가므로 $q_{k+1}$를 직접 풀어야 한다.
일반적으로 비선형 방정식 풀이가 필요하다.
대신 안정성이 매우 좋다.

선형 감쇠식에서는 $u_{k+1}=\dfrac{1}{1+\tau a}u_k$가 되어 항상 감쇠한다.
문제는 원래 시스템에 없던 수치 감쇠가 생길 수 있다는 점이다.
즉 인공적인 감쇠가 생겨서, 진자의 에너지가 지나치게 줄어드는 식이다.

### Symplectic Euler

역학계에서 자주 쓰이는 절충안이다.
보통 속도는 현재 상태로 업데이트하고, 그다음 상태는 새 속도로 업데이트한다.
예를 들면 $$v_{k+1}=v_k+\tau a(q_k,v_k), \qquad q_{k+1}=q_k+\tau v_{k+1}$$와 같다.

Forward Euler보다 안정적이고, Backward Euler처럼 에너지를 과도하게 잃지도 않는다.
장시간 시뮬레이션에서 에너지 거동이 좋아서 실무에서도 자주 쓰인다.
leapfrog, Verlet와 함께 기억해두면 좋다.
이제 진자는 거의 정확하게 에너지를 보존하며 영원히 움직인다.

### Choosing Integrators

좋은 적분기의 기준은 하나가 아니다.
안정성(stability), 정확도(accuracy), 일관성(consistency), 수렴성(convergence), 보존성(conservation), 대칭성(symmetry), 계산 효율(computational efficiency)을 함께 봐야 한다.

따라서 모든 문제에 대해 최고인 한 가지 방법이 있는 것은 아니다.
시스템의 성질과 목표에 맞게 고르는 문제다.

### Derivatives in Simulation

시뮬레이션에서는 에너지에서 힘을 얻거나, 최적화기와 솔버를 만들기 위해 도함수가 자주 필요하다.

다변수 함수 $f:\mathbb R^n\to\mathbb R$에서 방향 미분은 $$D_X f(q_0)=\lim_{\epsilon\to 0}\dfrac{f(q_0+\epsilon X)-f(q_0)}{\epsilon}$$이다.
그래디언트 $\nabla f(q_0)$는 이 방향 미분을 내적으로 표현하는 벡터다.
즉 $$\langle \nabla f(q_0), X\rangle = D_X f(q_0)$$를 만족한다.

위치 에너지 $U$를 쓰는 식에서는 힘을 보통 $F=-\nabla U$로 둔다.

### Differentiation Methods

도함수를 구하는 방법도 여러 가지다.

>수치 미분(numerical differentiation)은 

$$f'(x_0)\approx \dfrac{f(x_0+h)-f(x_0)}{h}$$처럼 차분으로 근사한다.

![](../assets/images/Pasted%20image%2020260316235410.png)
코드는 단순하지만 $h$를 너무 작게 잡아도 반올림 오차(round-off error) 때문에 오히려 결과가 나빠질 수 있다.

>자동미분(automatic differentiation)은 값과 도함수를 함께 전달한다.

예를 들어 $$(u,u')(v,v')=(uv,uv'+vu')$$처럼 계산한다.
정확도가 좋고 꽤 빠르지만, 연산 규칙을 시스템 안에 넣어야 한다.

>기호 미분(symbolic differentiation)은 식 트리를 변형해서 도함수를 만든다.
![](../assets/images/Pasted%20image%2020260320114531.png)
한 번 식을 만들어두면 반복 계산에 유리하지만, 결과 식이 지나치게 길어지기 쉽다.

기하적 미분(geometric differentiation)은 식을 기하적으로 다시 해석해서 더 짧고 의미 있는 도함수를 얻는 방식이다.
복잡한 symbolic 결과를 크게 단순화해준다.

### 요약

핵심 흐름은 상태를 정하고, ODE로 표현하고, 적절한 적분기로 시간을 전진시키는 것이다.
에너지 기반 모델링과 미분 계산은 물리 애니메이션의 핵심 도구다.
단순한 로컬 규칙과 간단한 모델만으로도 flocking, cloth, hair, crowds처럼 매우 풍부한 움직임이 만들어진다.
좋은 simulation은 물리 모델, 수치 적분, 미분 계산이 함께 맞물릴 때 나온다.
