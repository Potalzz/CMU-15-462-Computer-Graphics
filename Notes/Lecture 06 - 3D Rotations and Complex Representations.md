This post is a personal study note based on CMU 15-462 (Computer Graphics) 2020/fall lectures.
>All original lecture slides and videos are copyrighted by the instructors.

강의에 대한 정보와 자료는 아래 홈페이지에서 확인할 수 있습니다.
https://15462.courses.cs.cmu.edu/fall2020/home

본 포스팅은 강의 내용을 바탕으로 하되, 이해를 돕기 위해 별도의 자료 조사와 개념 정리를 덧붙여 작성했습니다. 따라서 원 강의 내용에 추가적으로 개인적인 학습 자료가 다수 포함되어 있습니다.

---

## 3D Rotations

## 교환 법칙(Commutativity)
**2D에서는 교환법칙이 성립**
![](../assets/images/Pasted%20image%2020260223131152.png)
2D에서는 회전 순서를 바꿔도 결과가 같음

**3D에서는 교환 법칙 성립X**
3D에서는 회전 순서가 결과를 바꾼다.
- Y로 90° → Z로 90° → X로 90°
- Z로 90° → Y로 90° → X로 90°  
이 둘은 일반적으로 결과가 다름.
(물병과 같은 물건을 가지고 직접 해보면 바로 알 수 있음)

## 2D 회전 표현
2D 회전 행렬을 그냥 암기하지 말고, **기저 벡터가 어떻게 움직이는지**로부터의 과정을 이해하기.

![](../assets/images/Pasted%20image%2020260223130553.png)
- 원 위 점을 주는 함수 $S(\theta)$를 생각하자. (아직 sin/cos를 몰라도 됨)
- $e_1$을 $\theta$만큼 회전시키면 $\tilde e_1 = S(\theta)$
- $e_2$를 $\theta$만큼 회전시키면 $\tilde e_2 = S(\theta + \pi/2)$

그러면 회전 행렬의 열은 어떤 모습이 될까?
$$
R(\theta) = [S(\theta)\;\;S(\theta+\pi/2)]
$$
결국 익숙한 형태로 연결된다.(왜 이렇게 되는지는 위 이미지를 보면 이해하기 쉬움.)
$$
=\space

\begin{bmatrix}
\cos\theta & \cos(\theta+\pi/2)\\
\sin\theta & \sin(\theta+\pi/2)
\end{bmatrix}

=\space


\begin{bmatrix}
\cos\theta & -\sin\theta\\
\sin\theta & \cos\theta
\end{bmatrix}
$$

## Euler Angles (오일러 각)
단순히 X/Y/Z 축에 대해 차례로 돌리면 3D 회전아닌가 ?
이 방식이 **Euler angles**.

오일러의 장점은 개념적으로 이해하기 쉽다는 것 말고 없음.

### 오일러 각의 문제점 - Gimbal Lock
오일러 각을 쓰다 보면, 특정 자세에서 **세 축 중 하나의 회전이 사실상 사라지는** 현상인 **짐벌 락**이 발생함.

![](../assets/images/Pasted%20image%2020260228150110.png)
각 축에 대한 회전 행렬이 있다.

![](../assets/images/Pasted%20image%2020260228150135.png)
행렬들의 곱이 오일러 각에 의한 회전을 나타냄.

![](../assets/images/Pasted%20image%2020260228150205.png)
$\theta_y = \pi / 2$의 경우를 살펴보자.

![](../assets/images/Pasted%20image%2020260228150317.png)
앞 행렬을 단순화하면, $\theta_x,\theta_z$를 조절해도 **어떤 한 평면에서만 도는 형태**로 축이 겹쳐버린다.


## Complex Analysis - Motivations
- 2D 기하 변환을 인코딩하는 자연스러운 방법
- 코드/표기/디버깅/사고 단순화
- 연산비용/대역폭/저장공간 어느 정도 절약
- 복소해석에 익숙해지면 새로운 문제 해법으로 이어질 수 있음

![](../assets/images/Pasted%20image%2020260228150712.png)

**허수를 이상하게 생각하지 말자**
허수라고 생각하지 말고, 새로운 연산을 정의한다고 생각.

**허수 단위 $i$의 기하학적 의미**
$$
i = \sqrt{-1}
$$
허수라고 생각하면 머리가 복잡해질 것임. 이러한 정의를 잊어버리고 그냥 단순하게 생각하기

![](../assets/images/Pasted%20image%2020260223171942.png)
즉, $i$는 축이나 미지의 수라기보다 반시계 방향으로 90도 회전하는 **연산자**처럼 보기.

![](../assets/images/Pasted%20image%2020260223172129.png)
복소수는 결국 2차원 벡터다.
기저 $(e_1,e_2)$ 대신 $(1,i)$를 쓴 2D 공간이라고 생각하면 된다.

복소수 곱셈은 **각도는 더해지고**, **크기는 곱해진다**.

#### Euler 공식

복소수는 지수 형태로 쓰면 더 깔끔해진다
$$
e^{l\theta}=\cos(\theta)+l\sin(\theta)
$$
복소수 지수 계산을 한다는 건 뭘 의미할까 ?
벡터를 올린다는게 무슨 뜻인가 ?

$e^{l\theta}$는 단지 세타 값을 단위 원 위의 한 점으로 변환하는 함수 일 뿐이다.

$e^{l\theta}$를 **지수 함수로 생각하지 말고**, x축으로부터 세타만큼 떨어진 원 위의 점을 알려주는 함수라고 생각하자.

이를 이용하여 복소수 곱을 구현할 수 있다.

$$
z_1 = ae^{l\theta}, z_2 = be^{l\theta}
$$
$$
z1_z2 = abe^{l(\theta\phi)}
$$

### 2D 회전에서 행렬 vs 복소수
그래서 왜 2D 좌표계 회전에서 복소수를 사용해야 할까?

**벡터 $u$를 $\theta$만큼 회전시키고, 다시 $\phi$만큼 회전**시키는 상황을 비교하해보자.

- 행렬: $BAu$로 들어가면서 계산이 복잡해진다.
- 복소수: 각도가 $\alpha+\theta+\phi$로 **깔끔하게 누적**된다.

![](../assets/images/Pasted%20image%2020260226163518.png)

## Quaternions (쿼터니언)
>3D 회전을 (복소수처럼) 곱셉으로 다루기 위해 만든 4차원 수 체계

**Hamilton's insight**
2D에서 복소수로로 회전하듯, 3D회전을 하려면 4개의 좌표가 필요하다.

실수 1개, 허수 3개
$$ \mathbb{H}:=\text{span}({1,i,j,k})\ q=a+bi+cj+dk\in\mathbb{H} $$
$$
q = a + bi + cj + dk
$$
- $a$: 스칼라(실수부)
- $(b, c, d)$: 벡터처럼 취급하는 3개 성분(허수부)
- 곱셈 규칙이 추가로 필요. $i^2=j^2=k^2=ijk=-1$
- 곱셈은 교환법칙이 성립하지 않는다 ($qp \neq pq$)

두 쿼터니언 곱을 성분으로 전개할 수는 있는데 식이 매우 복잡하다.
![](../assets/images/Pasted%20image%2020260228125604.png)


![](../assets/images/Pasted%20image%2020260228135747.png)

### Quaternions - Scalar + Vector 형태
성분이 4개면 3D의 점/벡터를 어떻게 다룰까?
-> 허수 부분이 3개(i, j, k)니까 3D 벡터를 인코딩하는데 사용.
$$
(x, y, z) \rightarrow 0 +xi+yj+zk
$$

쿼터니언을 `(scalar, vector)` 묶음으로 보면 쿼터니언 곱이 훨씬 단순해짐.
$$
q=(a,\mathbf{u}) \space ,\mathbf{u}=(b,c,d)
$$
$$
(a,\mathbf{u})(b,\mathbf{v}) = (ab-\mathbf{u}\cdot\mathbf{v},\; a\mathbf{v}+b\mathbf{u}+\mathbf{u}\times\mathbf{v})
$$
($\mathbf{u} \cdot \mathbf{v}$은 내적, $\mathbf{u} \times \mathbf{v}$ 는 외적)

특히 순수 벡터(실수부 0)끼리는 더 단순
$$
\mathbf{u}\mathbf{v} = \mathbf{u}\times\mathbf{v} - \mathbf{u}\cdot\mathbf{v}
$$

### 쿼터니언으로 3D 변환
그래픽스에서 쿼터니언의 대표 용도는 **회전**

축 $\mathbf{u}$, 각 $\theta$가 주어졌을 때, 회전을 나타내는 쿼터니언 q는
![](../assets/images/Pasted%20image%2020260228142443.png)

행렬 공식보다 훨씬 외우기 쉽고, 조작도 간단하다.

## 회전 보간(Interpolating Rotations): SLERP
쿼터니언이 왜그렇게 유용하냐?
-> **회전 보간을 정말 쉽게 해줌.**

이전 강의에서 **polar decomposition**을 통해 회전을 보간하는 방법을 살펴보았지만,
그렇게 얻은 회전은 일정한 속도로 회전하지 않았음.

쿼터니언에서는 **SLERP**(spherical linear interpolation)공식을 쓴다
![](../assets/images/Pasted%20image%2020260228142819.png)
![](../assets/images/Pasted%20image%2020260228142939.png)



## 복소수(Complex Number)는 어디에 또 쓰일까?
### 1) 텍스처 맵을 위한 좌표 생성
복소수는 **각 보존(conformal)** 변환을 다루는 데 자연스럽다.  
사람의 지각은 각 왜곡에 민감해서, 텍스처 좌표를 만들 때 각을 잘 보존하면 보기 좋은 경우가 많다.

![](../assets/images/Pasted%20image%2020260228143526.png)


### 2) 프랙탈(쓸모없지만 아름다운 예시)
복소수의 반복으로 정의되는 프랙탈은, 그래픽스에서 **구조가 반복되는 복잡한 패턴**을 만드는 대표 사례.
![](../assets/images/Pasted%20image%2020260228143625.png)


## 요약
**3D에서 회전은 생각보다 복잡함**

**일반적으로 가능한 표현들은 많음**
- Euler angles: 직관적이지만 **gimbal lock**, 보간 문제
- axis-angle: 개념이 깔끔하지만 행렬은 외우기/조작이 번거로울 수 있음
- quaternions: 4성분이지만 회전/보간에서 강력(특히 SLERP), gimbal lock 회피

>**정답이나 최선의 방법은 없음. 많이 알 수록 더 많은 일을 할 수 있음.**
