>This post is a personal study note based on CMU 15-462 (Computer Graphics) 2020/fall lectures.
>All original lecture slides and videos are copyrighted by the instructors.

강의에 대한 정보와 자료는 아래 홈페이지에서 확인할 수 있습니다.
https://15462.courses.cs.cmu.edu/fall2020/home

본 포스팅은 강의 내용을 바탕으로 하되, 이해를 돕기 위해 별도의 자료 조사와 개념 정리를 덧붙여 작성했습니다. 따라서 원 강의 내용에 추가적으로 개인적인 학습 자료가 다수 포함되어 있습니다.

---

그래픽스에서 적분이 왜 필요한지, 고정된 sample 기반 적분과 Monte Carlo가 왜 중요한지 중심으로 정리.

## Motivation

- 렌더링에서는 한 점에서 모든 방향으로부터 들어오는 빛을 합쳐야 한다.
- 이 합이 연속적인 방향 전체에 대해 이루어지면 적분이 된다.
- 대표 예시가 rendering equation.

$$
L_o(p,\omega_o)=L_e(p,\omega_o)+\int_{\mathcal{H}^2} f_r(p,\omega_i\to\omega_o)L_i(p,\omega_i)\cos\theta\,d\omega_i
$$

- 의미는 단순하다. 나가는 빛은 스스로 내는 빛 + 모든 incoming direction에서 들어온 빛의 총합.
	- ![](../assets/images/Pasted%20image%2020260312004807.png)
- 그래픽스에서 적분으로 쓰는 양은 많다. total brightness, total area, total curvature 등.
- 아주 단순한 적분만 해석적으로 계산 가능. 실제 문제에서는 수치적 근사가 필요.

## Integral View

- 적분은 곡선 아래 넓이.
	- ![](../assets/images/Pasted%20image%2020260312005002.png)
- 또 다른 관점은 구간 평균값 $\times$ 구간 길이.
- 이 평균값 관점이 Monte Carlo 설명으로 이어진다.

$$
\int_a^b f(x)\,dx
$$

- fundamental theorem of calculus도 핵심.

$$
F'(x)=f(x) \quad\Rightarrow\quad \int_a^b f(x)\,dx = F(b)-F(a)
$$

- 상수 함수는 가장 쉽다.
$$
\int_a^b C\,dx = (b-a)C
$$
	- ![](../assets/images/Pasted%20image%2020260312005129.png)

- affine 함수는 양 끝값 평균만 알면 적분 가능.

$$
\int_a^b f(x)\,dx = \frac{f(a)+f(b)}{2}(b-a)
$$
	- ![](../assets/images/Pasted%20image%2020260312005144.png)

- 즉, 적분을 정확히 하거나 근사하려면 어디서 샘플할지와 각 샘플을 얼마나 반영할지가 중요.

## 구적법(Quadrature)

- 적분 근사는 결국 quadrature points와 weights의 문제.

$$
\int_a^b f(x)\,dx \approx \sum_i w_i f(x_i)
$$

- $x_i$는 어디서 측정할지.
- $w_i$는 그 측정값이 얼마나 큰 비중을 가지는지.
- **가우스 구적법**
  차수 n인 임의의 다항식에 대해, 특수한 n개의 점에서 샘플링하고 적절한 weight를 취하면 적분값을 정확히 얻을 수 있다.
	- ![](../assets/images/Pasted%20image%2020260312005711.png)
- piecewise affine 함수는 각 구간을 따로 적분한 뒤 전부 더하면 된다.
	- ![](../assets/images/Pasted%20image%2020260312005559.png)
	- ![](../assets/images/Pasted%20image%2020260312005624.png)
- 핵심은 적분을 유한한 개수의 함수 평가로 바꾸는 것

## 사다리꼴 법칙(Trapezoid Rule)
![](../assets/images/Pasted%20image%2020260312005828.png)

- arbitrary function을 작은 구간마다 직선으로 이어 붙인다고 생각하는 방법.
- 각 작은 구간을 사다리꼴로 보고 넓이를 더한다.

등간격 sample $x_0, x_1, \dots, x_N$와

$$
h = \frac{b-a}{N}
$$

를 쓰면,

$$
\int_a^b f(x)\,dx \approx h\left(\frac{1}{2}f(x_0)+\sum_{i=1}^{N-1}f(x_i)+\frac{1}{2}f(x_N)\right)
$$

- 끝점은 절반 weight, 내부 점은 full weight.
- 함수가 충분히 매끄러우면 error는 $O(h^2)=O(1/N^2)$ 수준.
- 필요한 work는 sample 개수에 비례하므로 $O(N)$.
- 1D에서는 꽤 강력한 방법.

## High Dimensions

- 2D 함수 $f(x,y)$도 같은 생각으로 적분 가능.
- 다만 x 방향에 한 번, y 방향에 한 번 rule을 적용해야 한다.
- sample이 $N \times N$ grid가 되므로 work는 $O(N^2)$.
- 그런데 error 차수는 여전히 크게 좋아지지 않는다.

일반적으로 $k$차원에서는,

- 한 축당 $N$개 sample이면 work는 $O(N^k)$.
- 총 sample 수를 $M=N^k$라고 두면 error는 대략 $O(M^{-2/k})$.

차원이 커질수록 sample 수가 폭발하는 현상. curse of dimensionality, 차원의 저주.

- 렌더링은 direction, bounce, path parameter가 많아서 본질적으로 고차원 문제.
- 그래서 단순 grid 기반 quadrature는 잘 안 스케일한다.

## Monte Carlo Integration

- 고정된 grid 대신 random sample을 사용해서 적분을 추정하는 방법.
- 실행할 때마다 결과가 조금씩 달라질 수 있다.
- 하지만 같은 방법을 여러 번 반복해 평균을 보면 정답 쪽으로 간다.
- 차원이 높아져도 적용 방식이 크게 바뀌지 않는 점이 중요.

가장 단순한 uniform sampling 버전은 평균값 관점으로 이해할 수 있다.

도메인 $D$의 크기가 $|D|$이고, $X_1,\dots,X_n$을 $D$에서 균일하게 뽑으면

$$
\int_D f(x)\,dx \approx |D|\cdot \frac{1}{n}\sum_{i=1}^n f(X_i)
$$

- 적분 = domain 크기 $\times$ 함수 평균값.
- Monte Carlo는 그 평균값을 random sample의 평균으로 대신 구하는 셈.
- 함수가 끊기거나 모양이 복잡해도 적용 가능.
- 함수 식을 몰라도, 값만 평가할 수 있으면 쓸 수 있다.
- 대표적인 오차 감소 속도는 $O(n^{-1/2})$.
- 1D에서는 trapezoid rule이 더 좋을 수 있다.
- 하지만 고차원에서는 Monte Carlo가 훨씬 현실적.

## Probability Basics

Monte Carlo를 쓰려면 확률 개념이 필요.

- random variable $X$: 여러 가능한 값을 갖는 변수.
- PDF $p(x)$: 어떤 값이 나올 상대적 가능성을 나타내는 함수.
- CDF $P(x)$: 누적 확률.

### Discrete Case

- 가능한 값이 유한 개일 때.
- 예: 주사위

각 값 $x_i$의 확률이 $p_i$이면,

$$
p_i \ge 0, \qquad \sum_i p_i = 1
$$

누적분포는

$$
P_j = \sum_{i=1}^j p_i
$$

- sample 방법도 단순.
- $\xi \in [0,1)$를 uniform하게 뽑고,

$$
P_{i-1} < \xi \le P_i
$$

를 만족하는 구간의 $x_i$를 선택.

### Continuous Case

- 가능한 값이 연속적일 때.
- 이때는 확률 자체보다 density를 적분해서 확률을 얻는다.

$$
p(x) \ge 0, \qquad \int p(x)\,dx = 1
$$

CDF는

$$
P(x)=\int_0^x p(s)\,ds
$$

구간 확률은

$$
\Pr(a\le X\le b)=\int_a^b p(x)\,dx = P(b)-P(a)
$$

- discrete에서는 누적합, continuous에서는 누적적분.

## Inversion Sampling
![](../assets/images/Pasted%20image%2020260312010258.png)

- continuous random variable을 sample하는 기본 방법.
- 먼저 CDF $P(x)$를 구하고, 그 inverse를 이용한다.

$$
\xi \sim U[0,1) \quad\Rightarrow\quad x=P^{-1}(\xi)
$$

- 조건은 두 가지.
  - CDF를 구할 수 있어야 한다.
  - inverse를 계산하거나 근사할 수 있어야 한다.


$$
p(x)=3(1-x)^2, \qquad x\in[0,1]
$$

이었고,

$$
P(x)=\int_0^x 3(1-x)^2\,dx = s^3-3x^2+3s
$$

inverse는

$$
x = 1-(1-y)^{1/3}
$$

- 즉, uniform random number를 적절히 변형하면 원하는 분포로 sample 가능.
- 좋은 sampling distribution을 설계하는 기본 도구.

## Disk Sampling

unit disk 안에서 점을 균일하게 뽑는 문제는 Monte Carlo의 감각을 보여주는 좋은 예.

### Wrong Sampling

다음 방식은 겉보기에는 그럴듯하지만 면적으로 균일하지 않다.

$$
\theta = 2\pi\xi_1, \qquad r = \xi_2
$$

그리고

$$
(x,y)=(r\cos\theta, r\sin\theta)
$$

- radius를 uniform하게 뽑으면 중심 근처에 점이 과하게 몰린다.
- 이유는 polar coordinates의 면적 요소가 $r\,dr\,d\theta$이기 때문.

### Correct Sampling

- 각도는 여전히 uniform.
- 반지름은 area에 맞게 더 바깥쪽이 자주 나오도록 뽑아야 한다.

$$
p(r)=2r, \qquad P(r)=r^2, \qquad r=\sqrt{\xi_2}
$$

따라서 올바른 방식은

$$
\theta = 2\pi\xi_1, \qquad r = \sqrt{\xi_2}
$$

$$
(x,y)=(r\cos\theta, r\sin\theta)
$$

- 확률값 자체가 균일해야 하는 것이 아니라, 최종 sample의 공간 분포가 균일해야 한다는 점이 중요.
- ![](../assets/images/Pasted%20image%2020260312010433.png)

## Rejection Sampling

- inversion이 어렵거나 inverse CDF를 모르더라도 sample할 수 있는 단순한 방법.
- 먼저 다루기 쉬운 큰 영역에서 uniform sample을 뽑는다.
- 원하는 영역 밖이면 버리고, 안이면 채택.

unit disk 예시:

- 정사각형 $[-1,1]\times[-1,1]$에서 uniform sample
- 원 밖이면 reject
- 원 안이면 accept

효율은

$$
\frac{\text{area of circle}}{\text{area of square}} = \frac{\pi}{4}
$$

- 구현은 매우 쉽다.
- 목표 영역이 큰 비율을 차지하면 꽤 쓸 만하다.
- 반대로 영역이 매우 얇거나 꼬여 있으면 거의 다 버리게 된다.
- 그럴 때는 rejection보다 변수 변환으로 분포를 맞추는 쪽이 낫다.
- ![](../assets/images/Pasted%20image%2020260312010458.png)

## 요약

- 적분 근사는 sample points와 weights로 정리된다.
- 차원이 낮고 함수가 매끄러우면 고정된 sample 기반 quadrature가 강하다.
- high dimension에서는 차원의 저주 때문에 grid 방식이 급격히 비싸진다.
- Monte Carlo는 random sampling으로 이 문제를 우회한다.
- Monte Carlo의 성능은 sample 수뿐 아니라 sample distribution 설계에도 크게 좌우된다.
- 렌더링에서는 결국 빛의 경로를 어떻게 잘 sample하느냐가 핵심.
