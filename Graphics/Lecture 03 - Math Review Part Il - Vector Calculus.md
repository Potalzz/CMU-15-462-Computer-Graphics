>This post is a personal study note based on CMU 15-462 (Computer Graphics) 2020/fall lectures.
>All original lecture slides and videos are copyrighted by the instructors.

강의에 대한 정보와 자료는 아래 홈페이지에서 확인할 수 있습니다.
https://15462.courses.cs.cmu.edu/fall2020/home

본 포스팅은 강의 내용을 바탕으로 하되, 이해를 돕기 위해 별도의 자료 조사와 개념 정리를 덧붙여 작성했습니다. 따라서 원 강의 내용에 추가적으로 개인적인 학습 자료가 다수 포함되어 있습니다.

---

### 벡터 미적분학 (Vector Calculus)
> 벡터 미적분학은 spatial relationships, transformations 등을 이야기하기 위한 기본 언어이다.

현대 컴퓨터 그래픽스의 상당 부분이 편미분 방정식(PDEs)으로 공식화된다.


### 유클리드 노름(Euclidean Norm)
> 유클리드 노름은 공간의 회전/이동/반사에 의해 보존되는 길이의 개념.

정규 직교(orthonormal) 좌표계에서의 **유클리드 노름**
$$|u|:=\sqrt{u_{1}^{2}+\cdot\cdot\cdot+u_{n}^{2}}$$
**주의해야 할 부분**
위 표현식은 벡터가 정규 직교 기저로 인코딩되지 않는 한 기하학적 길이를 인코딩하지 않는다.
`(버그의 흔한 원인)`


### 유클리드 내적(Euclidean Inner Product / Dot Product)

![](../assets/images/Pasted image 20260129172436.png)
n차원 벡터에 대해, 유클리드 내적은 다음과 같이 정의된다.
$$\langle u,v\rangle:=|u||v|cos(\theta)$$

정규 직교 데카르트 좌표계에서는 **점곱(dot product)** 을 통해 표현될 수 있다.
$$u\cdot v:=u_{1}v_{1}+\cdot\cdot\cdot+u_{n}v_{n}$$

**주의해야 할 부분**
유클리드 노름과 마찬가지로, 좌표가 정규 직교 기저에서 오지 않는 한 기하학적 의미가 없다.


### 외적(Cross Product)
내적이 **두 벡터가 얼마나 비슷한가**를 묻는 것이었다면,
3D에서 외적은 **두 벡터가 만드는 새로운 공간의 축은 어디인가**를 찾아내는 연산이며 "$u \times v$"로 쓴다.
내적의 결과는 숫자(스칼라)이지만, 외적의 결과는 또 다른 벡터이다.



![](../assets/images/Pasted image 20260129180022.png)

외적의 결과인 벡터는 **두 가지 정보**를 담고있다.
1. 화살표의 길이/크기: **평행사변형의 넓이**
2. 화살표의 방향: **평행사변형이 바라보는 방향**(수직)

**이 정의는 왜 3차원에서만 성립될까 ?**
2차원에서는 두 벡터가 평행을 이루어야만 두 벡터 모두에 직교할 수 있다.

**외적의 방향을 확인하는 방법**
![](../assets/images/Pasted image 20260131165002.png)
[오른 손 법칙]

다만, 오른손 법칙은 코드 상에서 사용하기 어려우므로 손이 필요 없는 방법이 필요하다.

### 외적(Cross Product), 행렬식(Determinant), and Angle

$$\sqrt{det(u,v,u\times v)}=|u||v|sin(\theta)$$
[더욱 정확한 정의(손이 필요 없는)]
- $\theta$는 u와 v 사이의 각도
- "det"는 세 열 벡터의 행렬식
- 좌표 공식을 고유하게 결정한다
	- ![](../assets/images/Pasted image 20260131165713.png)

#### Q. 왜 $u \times v = (u_1i, u2_j, u3_k) \times (v_1i, v_2j, v_3k)$식이 위의 수식이 될까 ?

**3차원 벡터 $u = (u_1, u_2, u_3)$는 사실 세 개의 화살표를 더한 것이다.**
- $x$축 방향 1칸: **$i$**
- $y$축 방향 1칸: **$j$**
- $z$축 방향 1칸: **$k$**


**따라서 벡터를 이렇게 풀어서 쓸 수 있다.**
$$u = u_1 i + u_2 j + u_3 k$$
$$v = v_1 i + v_2 j + v_3 k$$


**외적의 정의에 따라, 서로 직각인 축끼리 곱하면 다음과 같은 규칙이 생긴다.**
- **같은 축끼리 만나면 0이 된다.** (방향이 같으면 회전력이 안 생김)
    - $i \times i = 0$
    - $j \times j = 0$
    - $k \times k = 0$
      
- **다른 축끼리 만나면 나머지 하나가 된다.** (오른손 법칙 순서: $i \to j \to k \to i$)
    - $i \times j = k$
    - $j \times k = i$
    - $k \times i = j$
	
- **순서를 거꾸로 하면 마이너스(-)가 붙는다.**
    - $j \times i = -k$
    - $k \times j = -i$
    - $i \times k = -j$


**이제 두 벡터를 곱해보자.**
우리가 아는 곱셈 공식 $(a+b+c)(x+y+z)$ 처럼 하나씩 전개하면 총 9개의 항이 나온다.
$$u \times v = (u_1 i + u_2 j + u_3 k) \times (v_1 i + v_2 j + v_3 k)$$


**위 식을 전개해서, 위의 외적의 정의에 따른 규칙을 적용하면 다음과 같다.**
**같은 성분끼리의 곱 (3개 항) $\rightarrow$ 모두 0**
- $u_1 v_1 (i \times i) = 0$
- $u_2 v_2 (j \times j) = 0$
- $u_3 v_3 (k \times k) = 0$
- 결과: 0 $\rightarrow$ **모두 사라짐**

**$x$축($i$) 성분이 남는 경우**
- $u_2 j \times v_3 k = u_2 v_3 (j \times k) = u_2 v_3 (\mathbf{i})$
- $u_3 k \times v_2 j = u_3 v_2 (k \times j) = u_3 v_2 (\mathbf{-i})$
- 결과: $(u_2 v_3 - u_3 v_2) \mathbf{i}$ $\rightarrow$ **이게 공식의 첫 번째 줄**

**$y$축($j$) 성분이 남는 경우**
- $u_3 k \times v_1 i = u_3 v_1 (k \times i) = u_3 v_1 (\mathbf{j})$
- $u_1 i \times v_3 k = u_1 v_3 (i \times k) = u_1 v_3 (\mathbf{-j})$
- 결과: $(u_3 v_1 - u_1 v_3) \mathbf{j}$ $\rightarrow$ **이게 공식의 두 번째 줄.**

**$z$축($k$) 성분이 남는 경우**
- $u_1 i \times v_2 j = u_1 v_2 (i \times j) = u_1 v_2 (\mathbf{k})$
- $u_2 j \times v_1 i = u_2 v_1 (j \times i) = u_2 v_1 (\mathbf{-k})$
- 결과: $(u_1 v_2 - u_2 v_1) \mathbf{k}$ $\rightarrow$ **이게 공식의 세 번째 줄**

**결론**
이들을 모두 합쳐서 벡터 형태로 다시 쓰면 위의 공식이 나온다.
$$u \times v = \begin{bmatrix} u_2 v_3 - u_3 v_2 \\ u_3 v_1 - u_1 v_3 \\ u_1 v_2 - u_2 v_1 \end{bmatrix}$$


![](../assets/images/Pasted image 20260131171308.png)
원래 외적은 3차원에서 벡터를 반환하지만, 2차원에서도 $z$축이 있다 치고, 계산해서 나오는 숫자를 그냥 **2D 외적**이라고 퉁쳐서 부른다. 계산하거나 코딩할 때 간편하기에.
각도를 구하지 않고도(비싼 연산) 회전 방향과 좌우 위치를 확인하는 용도로 유용하게 사용된다.


### 90도 회전으로서의 외적
단위벡터 N과의 외적은 법선이 N인 평면에서의 90도 회전과 동일하다.
$$n \times u$$
평면 위 벡터를 90도 회전시키기 위한 목적이라면 복잡하게 회전 행렬을 사용하지  않고, 축과의 외적을 통해 간단하게 구할 수 있다.

![](../assets/images/Pasted image 20260131175729.png)
위 질문에 대해서, $\theta$만큼 회전한 벡터 $\mathbf{u}'$를 구하는 공식은 다음과 같다.
$$\mathbf{u}' = \mathbf{u}(\cos \theta) + (\mathbf{N} \times \mathbf{u})(\sin \theta)$$

![](../assets/images/Pasted image 20260131180039.png)
[공식이 성립되는 이유]

### 내적의 행렬 표현
**점곱을 행렬 곱을 통한 표현**
$$u\cdot v=u^{T}v=[\begin{matrix}u_{1}&\cdot\cdot\cdot&u_{n}\end{matrix}][\begin{bmatrix}v_{1}\\ \vdots\\ v_{n}\end{bmatrix}]=\sum_{i=1}^{n}u_{i}v_{i}$$

**다른 내적에 대해선 어떻게 표현할까 ?**
E.g., $<u,v>:=2u_1v_1+u_1v_2+u_2v_1+3u_2v_2$

![](../assets/images/Pasted image 20260131190117.png)

![](../assets/images/Pasted image 20260131190149.png)


### 외적의 행렬 표현
**행렬 곱셈을 통해 외적을 표현할 수도 있다.**
$$u:=(u_{1},u_{2},u_{3}) \Rightarrow \hat{u}:=\begin{bmatrix}0&-u_{3}&u_{2}\\ u_{3}&0&-u_{1}\\ -u_{2}&u_{1}&0\end{bmatrix}$$
성분 $(u_1, u_2, u_3)$를 가지는 3차원상의 벡터 $u$를 3 X 3 행렬로 변환한다.

#### Q. 왜 행렬로 바꿀까 ?
외적 기호는 산수에 가깝지만, 행렬은 선형대수이다.
식을 행렬로 바꿈으로써 역행렬, 미분 등의 수학적 도구들을 사용하고 결합 법칙을 사용하여 컴퓨터 그래픽스 연산 최적화에 필수적이다.

벡터가 행렬로 변함으로써 단순 **데이터**에서 **연산자**로 변하게 된다.

##### 변환 과정
**단계 1: 목표 설정**

우리는 어떤 행렬 $[\hat{u}]$를 찾고 싶다.
단, 이 행렬에 $v$를 곱했을 때의 결과가 $u \times v$와 똑같아야 한다.

$$[\hat{u}] \begin{bmatrix} v_1 \\ v_2 \\ v_3 \end{bmatrix} = \begin{bmatrix} -u_3 v_2 + u_2 v_3 \\ u_3 v_1 - u_1 v_3 \\ -u_2 v_1 + u_1 v_2 \end{bmatrix}$$

$$u\times v=\hat{u}v=\begin{bmatrix}0&-u_{3}&u_{2}\\ u_{3}&0&-u_{1}\\ -u_{2}&u_{1}&0\end{bmatrix}\begin{bmatrix}v_{1}\\ v_{2}\\ v_{3}\end{bmatrix}$$

**단계 2: 계수 추출 (인수분해)**

오른쪽 식에서 $v_1, v_2, v_3$를 기준으로 묶어낸다.

- **1행:** $(0)v_1 + (-u_3)v_2 + (u_2)v_3$
    
- **2행:** $(u_3)v_1 + (0)v_2 + (-u_1)v_3$
    
- **3행:** $(-u_2)v_1 + (u_1)v_2 + (0)v_3$


**단계 3: 행렬 배치**

위의 괄호 안의 숫자들을 그대로 행렬 칸에 옮겨 적으면 변환이 완료된다.
$$\hat{u} = \begin{bmatrix} 0 & -u_3 & u_2 \\ u_3 & 0 & -u_1 \\ -u_2 & u_1 & 0 \end{bmatrix}$$


#### Q. 새로운 행렬을 만들지 않고, $u \times v$ 를 어떻게 표현할 수 있을까 ?
$v \times u = -u \times v$임을 알면 유용하다.
외적은 일종의 반대칭이다.
연산에서 피연산자의 순서를 바꾸면 결과가 마이너스가 된다.

그러한 이유는 오른손 법칙을 생각해보면 쉽게 알 수 있다.
외적은 근본적으로 방향과 관련이 있으므로, 연산 순서가 바뀌면 방향이 정반대로 뒤집히기 때문이다.
![](../assets/images/Pasted image 20260131195756.png)

외적을 나타내는 데 사용하는 행렬은 반대칭 행렬이다.
`*반대칭 행렬: (주대각선의 원소는 0이며, 주대각선에 의하여 대칭인 위치에 있는 원소는 부호만 서로 반대.)`

따라서, $v$와 $u$를 교차하여 사용하려면 전치하는 방법이 있다.
$$v\times u=-\hat{u}v=\hat{u}^{T}v$$

### 행렬식, 부피와 삼중곱(Determinant, Volume and Triple Product)
#### 행렬식
행렬을 대표하는 **하나의 숫자**를 의미.
기하학적으로  **공간의 부피 확장 비율**을 의미한다.

행렬은 공간을 변형시키는데, 행렬식은 변형 과정에서 **영역의 크기가 몇 배로 변했는지**를 알려준다.

**행렬식의 부호의 의미**
행렬식의 숫자 값뿐만 아니라 **부호**도 중요하다.

- **양수 (+):** 정상적인 변환. (오른손 좌표계가 유지됨)
- **음수 (-):** 공간이 **뒤집힘**
- **0 (Zero):** **가장 중요한 상태**. 공간이 **납작하게 찌부러졌음**을 의미하며, 부피가 0이 되었음을 의미한다.(3차원 -> 2차원(면) 또는 1차원(선))
	- 행렬식이 0이되면 정보가 손실되어, 원래대로 되돌리는 것(역행렬)이 불가능함.
	- 역행렬이 존재X

![](../assets/images/Pasted image 20260131203852.png)
위와 같은 행렬의 행렬식에 대해서 기하학적 관점에서 살펴보자.



![](../assets/images/Pasted image 20260201184653.png)
$det(u,v,w)$는 모서리 벡터가 u, v, w인 평행육면체의 부피를 인코딩한다.(부피 정보에 방향 정보까지 합쳐서 하나의 숫자로 표현)

$$
\begin{align}
det(u,v,w)=(u\times v)\cdot w \\
=(v\times w)\cdot u \\
=(w\times u)\cdot v
\end{align}
$$
두 벡터의 외적은 평행사변형의 **면적(밑넓이)** 정보를 담고 있다.
이 외적 벡터에 나머지 벡터를 내적하면, 기하학적으로 **'밑넓이'** 와 **'높이(밑면에 수직인 방향의 성분)'** 를 곱하는 과정이 되어 최종적으로 **평행육면체의 부피**를 얻게 된다.
상자를 어느 면으로 눕혀도 부피는 변하지 않는 것처럼, 어떤 벡터 쌍을 밑면으로 정하든 계산 결과는 항상 동일하다.



### 행렬을 통한 선형 변환 표현(Representing Linear Maps via Matrices)

**선형 변환을 어떻게 행렬로 인코딩할 수 있을까 ?**
![](../assets/images/Pasted image 20260204122134.png)

$$f(u)=u_{1}a_{1}+u_{2}a_{2}+u_{3}a_{3}$$
[선형 변환]

$$A:=\begin{bmatrix}|&|&|\\ a_{1}&a_{2}&a_{3}\\ |&|&|\end{bmatrix}=\begin{bmatrix}a_{1,x}&a_{2,x}&a_{3,x}\\ a_{1,y}&a_{2,y}&a_{3,y}\\ a_{1,z}&a_{2,z}&a_{3,z}\end{bmatrix}$$
['a'벡터들을 행렬의 열로 하여 행렬로 인코딩]


$$A\begin{bmatrix}u_{1}\\ u_{2}\\ u_{3}\end{bmatrix}=\begin{bmatrix}a_{1,x}u_{1}+a_{2,x}u_{2}+a_{3,x}u_{3}\\ a_{1,y}u_{1}+a_{2,y}u_{2}+a_{3,y}u_{3}\\ a_{1,2}u_{1}+a_{2,2}u_{2}+a_{3,z}u_{3}\end{bmatrix}=u_{1}a_{1}+u_{2}a_{2}+u_{3}a_{3}$$
[행렬-벡터 곱셈이 원래 변환을 복원]


### 다른 삼중곱들(Other Triple Products)

**외적에 대한 자코비 항등식(Jacobi identity)**
![](../assets/images/Pasted image 20260204122943.png)
$$u\times(v\times w)+ v\times(w\times u)+ w\times(u\times v)=0$$

**라그랑주 항등식(Lagrange's identity)**
$$u\times(v\times w)=v(u\cdot w)-w(u\cdot v)$$


### 미분 연산자 (Differential Operators)
#### 미분 연산자와 벡터장(vector fields)이 왜 컴퓨터 그래픽스에서 유용한가 ?
많은 물리적/기하학적 문제들이 상대적인 변화율(상미분 방정식(ODEs), 편미분방정식(PDEs))로 표현된다.

**상미분방정식과 편미분방정식에 대한 간단한 정리**

| **구분**     | **상미분방정식 (ODEs)** | **편미분방정식 (PDEs)**               |
| ---------- | ----------------- | ------------------------------- |
| **변화의 원인** | 오직 **시간** ($t$)   | **시간**($t$) + **공간**($x, y, z$) |
| **관심 대상**  | 개별 입자, 단일 물체      | 연속체 (물, 공기, 천, 열, 빛)            |
| **주요 활용**  | 캐릭터 이동, 강체 물리, 탄성 | 유체 시뮬레이션, 연기, 사실적 조명            |
| **계산 방식**  | 상대적으로 가볍고 빠름      | 격자로 나누어 행렬로 푸는 복잡한 연산           |

미분 연산자는 수치 최적화의 근간(Foundations)을 제공한다.

**그래픽스에서의 최적화란 ?**
-> 가장 좋은 상태(에너지가 가장 낮거나, 오차가 가장 적은 지점)를 찾는 과정

**최적화 문제를 하나의 산(함수)에서 가장 낮은 골짜기를 찾는 과정이라고 비유해보자**

- **미분 연산자(Gradient)의 역할:** 내가 지금 서 있는 지점에서 어느 방향이 가장 가파른지를 알려준다.
  즉, **어느 방향으로 가야 에너지가 가장 빨리 줄어드는가?**라는 방향 지시등 역할을 함.
    
- **수치 최적화의 과정:** 미분 연산자가 알려준 방향(Gradient)을 따라 한 걸음씩 이동하며 최솟값을 찾아가는 반복적인 계산 과정.

결국에 정답을 도출하기 위해서는, 미분 -> 이동 -> 미분 의 과정을 반복해야 하는데, 미분 연산자를 통해 얻은 **기울기(Gradient)** 나 **곡률(Curvature)** 값을 토대로 계산을 시작할 수 있다.

![](../assets/images/Pasted image 20260204152621.png)


### 도함수 (Derivative)
![](../assets/images/Pasted image 20260204154441.png)
$$f^{\prime}(x_{0}):=lim_{\epsilon\rightarrow0}\frac{f(x_{0}+\epsilon)-f(x_{0})}{\epsilon}$$

![](../assets/images/Pasted image 20260204155218.png)
위와 같이, 오른쪽에서 다가올 때와 왼쪽에서 다가올 때 기울기가 다르면 미분 불가능

우미분계수 ($f^+(x_0)$) = 좌미분계수 ($f^-(x_0)$)이면 미분 가능



### 최적 선형 근사에서의 도함수 (Derivative as Best Linear Approximation)
무한히 미분 가능한 함수 f(x)는 테일러 급수(Taylor series)로 표현될 수 있다.
$$f(x)=f(x_{0})+f^{\prime}(x_{0})(x-x_{0})+\frac{(x-x_{0})^{2}}{2!}f^{\prime\prime}(x_{0})+\cdot\cdot\cdot$$
[상수항 + 선형항 + 이차항 ...]

**테일러 급수**는 쉽게 말해 복잡한 함수를 다항식(더하기, 곱하기)으로 이루어진 가짜 함수로 대체하는 도구.
그래픽스에서 계산 효율을 위해 곡선을 직선(1차)이나 포물선(2차)으로 단순화해서 처리할 때 사용한다.


### 방향 도함수 (Directional Derivative)
>임의의 방향에 대한 기울기 값(스칼라)

![](../assets/images/Pasted image 20260205151506.png)

**일반적인 도함수**은 특정 축 방향(x축, y축)으로만 갔을 때의 기울기를 묻는 것이지만,
- **ex)** 오직 x축 방향으로만 이동했을 때의 기울기
**방향 도함수**은 임의의 방향으로 이동했을 때의 경사를 의미한다.
- **ex)** 37.5도 방향으로 이동했을 때의 기울기
  
위에서 slice된 이미지는, 가고자 하는 방향으로 단면을 자르고, 절단면의 곡선에서 기울기를 보겠다는 것을 의미한다.

위의 수식을 뜯어서 살펴보자.
$$D_{\mathbf{u}}f(\mathbf{x}_0) = \lim_{\epsilon \to 0} \frac{f(\mathbf{x}_0 + \epsilon \mathbf{u}) - f(\mathbf{x}_0)}{\epsilon}$$

- $D_{\mathbf{u}}$: 방향 도함수를 의미. $u$방향으로 미분을 하겠다
	
- **$\mathbf{x}_0$**: 현재 내가 서 있는 위치
    
- **$\mathbf{u}$**: 내가 이동하려는 **방향**을 나타내는 화살표(벡터) (보통 길이는 1로 고정)
    
- **$\epsilon \mathbf{u}$**: 방향 $\mathbf{u}$로 $\epsilon$만큼 아주 살짝 움직인 거리
    
- **$f(\mathbf{x}_0 + \epsilon \mathbf{u})$**: 아주 살짝 움직인 곳에서의 **높이**
    
- **$f(\mathbf{x}_0)$**: 원래 서 있던 곳의 **높이**

결과적으로 함수 f의 모든 가능한 방향 u에 따른 변화를 의미한다.
따라서 방향 도함수는 **그라디언트($\nabla f$)를 내가 가고자 하는 방향 벡터($\mathbf{u}$)에 투영한 값**과 같다.

### 기울기 (Gradient)
>Gradient($\nabla f$)는 **가장 가파르게 값이 증가하는 방향과 그 크기**를 나타내는 벡터

![](../assets/images/Pasted image 20260205152956.png)
다변수 함수 $f(x)$가 주어졌을 때, 그라디언트 $\nabla f(x)$는 각 지점에 벡터를 할당한다.

그라디언트의 화살표가 왜 위와 같이 형성되는지에 대해서 시각적으로 잘 설명한 영상이 있으니 꼭 보는 것을 추천한다.
https://www.youtube.com/watch?v=GkB4vW16QHI


### 좌표계에서의 그라디언트 (Gradient in Coordinates)
가장 친숙한 정의 : **편미분**들의 리스트

방향 도함수는 특정 방향에 대한 1차원 미분을 제공.
그라디언트는 단지 방향 도함수를 리스트로 나타낸 것에 불과하다.
$$\nabla f = \begin{bmatrix} \frac{\partial f}{\partial x_1} \\ \frac{\partial f}{\partial u_2} \\ \vdots \\ \frac{\partial f}{\partial x_n} \end{bmatrix}$$

하지만 이렇게 생각하면 두 가지 문제점이 존재한다.
1. 내적의 역할이 명확하지 않다.(강의 뒷 부분에서 다시 얘기)
2. 유한한 좌표 리스트 $x_1, ..., x_n$이 없기 때문에, 함수들의 함수 $F(f)$를 미분할 방법이 없다.

그럼에도, 그라디언트를 계산하는 매우 일반적인 방법이다.

예시)
![](../assets/images/Pasted image 20260205161338.png)

함수 $f(x):=x_{1}^{2}+x_{2}^{2}$의 기울기 구하기.
위에서 그라디언트는 편미분들의 리스트라고 했으니 편미분을 진행해보자.
각 방향에 대해서 미분을 진행한다.(나머지 방향은 상수로 취급)
편미분한 결과를 결합한다.
$$\nabla f(x)=\begin{bmatrix}2x_{1}\\ 2x_{2}\end{bmatrix}=2x$$[최종적으로 위 결과를 얻을 수 있다.]

해당 함수의 그라디언트를 구했으므로, 특정 벡터를 넣으면 해당 위치에서의 그라디언트를 얻을 수 있다.

### 최적 선형 근사로서의 그라디언트 (Gradient as Best Linear Approximation)

![](../assets/images/Pasted image 20260206162927.png)
$$f(x)≈f(x0​)+⟨∇f(x0​),x−x0​⟩$$

각 지점 x0​에서, 그라디언트는 최적의 근사로 이끄는 고유한 벡터 $\nabla f(x_{0})$이다.

x0​에서 시작할 때, 이 항은
- 우리가 그라디언트 방향으로 움직이면 **더 커지고**,
- 반대 방향으로 움직이면 **더 작아지고**,
- 그라디언트에 직교(수직)하게 움직이면 **변하지 않는다**

![](../assets/images/Pasted image 20260206163216.png)

그라디언트는 가장 가파른 상승의 방향으로 이동하므로, 함수의 값을 가장 빠르게 증가시키려면 해당 방향으로 이동하면 된다.

이러한 관점은 그래픽스에서 일반적으로 사용되는 최적화(Optimazation)알고리즘으로 이어진다.

### 그라디언트와 방향 도함수  (Gradient and Directional Derivative)
$$\langle\nabla f(x),u\rangle=D_{u}f(x)$$
각 지점 x에서, 그라디언트는 모든 u에 대해 다음을 만족하는 유일한 벡터 $\nabla f(x)$이다.

그라디언트를 알고 있으면, 특정 방향으로 변화를 계산할 때, 단순히 그라디언트와의 내적을 취하면 된다.(함수가 미분 가능해야 한다.)


### 내적의 그라디언트(Gradient of Dot Product)
$$f:=u^{T}v$$
$u$와 $v$의 내적을 행렬 연산으로 표현.(행과 열 개수를 맞추기 위해 u를 전치)

**좌표를 통해 써보면서 $u$에 대한 $f$의 그라디언트($\nabla f$)를 구해보자.**
-> 벡터 $u$의 모든 성분($u_1, u_2, \dots, u_n$)에 대해서 각각 편미분을 수행해서 나열해라는 의미임.

$$u^{T}v=\sum_{i=1}^{n}u_{i}v_{i}$$
$u^Tv$는 각 항의 합과 같다.

$$\frac{\partial}{\partial u_{k}}\sum_{i=1}^{n}u_{i}v_{i}=\sum_{i=1}^{n}\frac{\partial}{\partial u_{k}}(u_{i}v_{i})=v_{k}$$
각각 편미분을 취하게 되면 $u_k$가 나온다.

$$\Rightarrow\nabla_{u}f=\begin{bmatrix}v_{1}\\ \cdot\cdot\cdot\\ v_{n}\end{bmatrix}$$
결과적으로 u에 대한 f의 그라디언트는 벡터 $v$인 것을 알 수 있다.
쉽게 말하면 $\nabla_{u}(u^{T}v)=v$이고 $\frac{d}{dx}(xy)=y$와 크게 다르지 않다.

>**그래픽스에서 행렬을 포함하는 표현식을 미분할 수 있는 것은 매우 유용하다.**

결국, 표현식들은 일반적인 미분과 매우 비슷해 보인다.
![](../assets/images/Pasted image 20260206175308.png)

이러한 원리를 이해하고 나면, 위와 같이 행렬을 좌표로 표현하는 번거로운 과정을 거칠 필요가 없어진다.


### $L^2$ Gradient (Advanced)
#### 문제 정의: 함수의 변화율 찾기
$f$를 입력으로 받는 함수 $F(f)$가 있다고 하자.
이떄, $F$의 값을 가장 빠르게 증가시키는 방향, 즉 그라디언트($\nabla F$) 는 무엇일까 ?

일반적인 미분과 달리, 우리가 다루는 공간은 유한한 좌표계($x, y, z \dots$)가 아닌 **무한 차원의 함수 공간**이다.
따라서 단순히 각 성분별로 편미분을 하는 방식으로는 해결할 수 없다.

#### 해결 도구: 방향 미분과 내적
이 문제를 해결하기 위해 **그라디언트의 정의**를 내적을 통해 다시 정립한다.
임의의 변화 함수 $u$에 대하여, 다음 관계를 만족하는 함수 $\nabla F$를 찾는 것이 목표이다.
$$\langle\langle\nabla F, u \rangle\rangle = D_uF$$
여기서 $D_uF$는 $u$방향으로의 방향 미분(Directional Derivative)이다.

참고로, "$\langle \langle$" 이 두 개짜리 꺽쇠는 **함수의 내적**을 의미한다.공간이 벡터 공간 -> 함수 공간으로 확장됨에 따라 내적의 정의도 자연스럽게 확장된다.

- **벡터의 내적:** 성분끼리 곱해서 더함($\sum$)$$\langle u, v \rangle = \sum u_i v_i$$
- **함수의 내적:** 함수값끼리 곱해서 적분($\int$)$$\langle\langle f, g \rangle\rangle = \int_{\Omega} f(x)g(x) \, dx$$
본질적으로 두 대상의 **유사도(Alignment)** 를 측정한다는 점은 동일함.


#### 그라디언트 유도 과정
구체적인 계산을 위해서 방향 미분의 정의를 활용하자.
$$D_{u}F(f)=lim_{\epsilon\rightarrow0}\frac{F(f+\epsilon u)-F(f)}{\epsilon}$$
$F$에 대해서 방향 미분을 한다고 이해하면 됨.

이제 두 가지 예시를 통해 실제로 그라디언트를 구해보자.

아래 예시를 살펴보자.
##### Case 1. 선형(Linear) 형태
$$F(f) := \langle\langle f, g \rangle\rangle$$
이 경우 그라디언트는 $g$이다
$$\nabla F=g$$
![](../assets/images/Pasted image 20260207001716.png)
해석: 내적은 두 함수의 유사도를 의미한다. $f$가 $g$와 가장 비슷해지는 방향으로 변해야 $F$값이 가장 빠르게 커진다.
따라서 $f$에 더해주어야 할 가장 좋은 재료(방향)는 바로 $g$ 자신이다.

##### Case 2. 이차(Quadratic) 형태
$$F(f) := \|f\|^2 = \langle\langle f, f \rangle\rangle$$
이 경우 각 지점 $f_0$에서의 그라디언트 $\nabla F(f_0)$를 구해보자.

**변화량 전개**
분자의 첫 번째 항인 $|f_{0}+\epsilon u|^{2}$을 전개한다. (곱셈 공식 $(a+b)^2$와 유사)$$\|f_{0}+\epsilon u\|^{2} = \|f_{0}\|^{2} + 2\epsilon\langle\langle f_{0}, u \rangle\rangle + \epsilon^{2}\|u\|^{2}$$
  
**극한 계산**
정의식에 대입하여 $\epsilon$을 $0$으로 보낸다. $\epsilon^2$ 항은 사라짐.  $$\lim_{\epsilon\rightarrow0} \frac{(\|f_0\|^2 + 2\epsilon\langle\langle f_0, u \rangle\rangle + \epsilon^2\|u\|^2) - \|f_0\|^2}{\epsilon} = 2\langle\langle f_{0}, u \rangle\rangle$$
**매칭**
모든 $u$에 대해 $\langle\langle \nabla F(f_{0}), u \rangle\rangle = 2\langle\langle f_{0}, u \rangle\rangle$를 만족해야 한다. 따라서 유일한 해는 다음과 같다.$$\nabla F(f_{0}) = 2f_{0}$$
각 지점 $f_0$에서, 우리는 모든 함수 u에 대해 다음을 만족하는 함수 $\nabla F$를 원한다.
$$\langle\langle\nabla F(f_{0}),u\rangle\rangle=lim_{\epsilon\rightarrow0}\frac{F(f_{0}+\epsilon u)-F(f_{0})}{\epsilon}$$

분자의 첫 번째 항을 전개하면$$||f_{0}+\epsilon u||^{2}=||f_{0}||^{2}+\epsilon^{2}||u||^{2}+2\epsilon\langle\langle f_{0},u\rangle\rangle$$

따라서, 극한은 다음과 같이 된다.$$lim_{\epsilon\rightarrow0}(\epsilon||u||^{2}+2\langle\langle f_{0},u\rangle\rangle)=2\langle\langle f_{0},u\rangle\rangle$$

모든 u에 대해 $\langle\langle\nabla F(f_{0}),u\rangle\rangle=2\langle\langle f_{0},u\rangle\rangle$를 만족하는 유일한 해는
  $$\nabla F(f_{0})=2f_{0}$$

결과적으로 $x^2$ 미분하면 $2x$ 되는 거랑 크게 다르지 않다.
기존에 알고 있는 미분 공식과 일치 한다.
$$\frac{d}{dx}x^{2}=2x$$

아무리 복잡해 보이는 함수 공간의 식이라도, 잘게 쪼개보면 우리가 알고 있는 직관적인 미분 규칙이 그대로 적용됨을 알 수 있다.


### 벡터장 (Vector Fields)
![](../assets/images/Pasted image 20260207020717.png)
그라디언트는 벡터장의 첫 번째 예시이다.
일반적으로, 벡터장은 공간의 각 지점에 벡터를 할당한다.

벡터장에 대한 **두 가지 기본 미분**이 존재한다.
- 장이 얼마나 수축/팽창하고 있는가? -> **발산 (Divergence)**
    
- 장이 얼마나 회전하고 있는가? -> **회전 (Curl)**
![](../assets/images/Pasted image 20260207021125.png)


#### 발산 (Divergence)
div X는 일반적으로 $\nabla\cdot X$으로도 표기된다.

발산에 대한 좌표 정의를 암시한다.

$\nabla$를 미분들의 벡터로 생각해보자.$$\nabla=(\frac{\partial}{\partial u_{1}},\cdot\cdot\cdot,\frac{\partial}{\partial u_{n}})$$
X를 함수들의 벡터로 생각해보자.$$X(u)=(X_{1}(u),...,X_{n}(u))$$
그러면 발산은 다음과 같다.$$\nabla\cdot X:=\sum_{i=1}^{n}\partial X_{i}/\partial u_{i}$$

#### 회전 (Curl)
curl X는 일반적으로 $\nabla\times X$으로도 표기된다.

회전에 대한 좌표 정의를 암시한다.

이번에는 $\nabla$를 딱 세 개의 미분 벡터로 생각해보자.(3차원의 경우)
$$\nabla=(\frac{\partial}{\partial u_{1}},\frac{\partial}{\partial u_{2}},\frac{\partial}{\partial u_{3}})$$X를 세 함수들의 벡터로 생각해보자.$$X(u)=(X_{1}(u),X_{2}(u),X_{3}(u))$$
그러면 회전은 다음과 같다.$$\nabla\times X:=\begin{bmatrix}\partial X_{3}/\partial u_{2}-\partial X_{2}/\partial u_{3}\\ \partial X_{1}/\partial u_{3}-\partial X_{3}/\partial u_{1}\\ \partial X_{2}/\partial u_{1}-\partial X_{1}/\partial u_{2}\end{bmatrix}$$
2D 회전: $\nabla\times X:=\partial X_{2}/\partial u_{1}-\partial X_{1}/\partial u_{2}$


#### 발산 vs 회전 (2D)
X의 발산은 X를 90도 회전시킨 것의 회전과 같다.
![](../assets/images/Pasted image 20260207022533.png)
$$\nabla \cdot X=\nabla\times X^{\perp}$$
발산과 회전은 완전히 다른 개념이 아니다. 상호보완적인 역할을 한다.
벡터장을 가지고 이런 종류의 게임을 하는 것은 알고리즘(ex. 유체 시뮬레이션)에서 중요한 역할을 한다.



### 라플라시안 (Laplacian)
>특정 지점이 주변 평균보다 얼마나 튀어나왔는지(볼록/오목)를 계산하는 도구

기하학, 렌더링, 시뮬레이션, 이미징 전반에 걸쳐 나타나는 그래픽스에서 중요한 객체
- 푸리에 변환 / 주파수 분해의 기초
- 모델 편미분방정식(라플라스, 열, 파동 방정식)을 정의하는 데 사용됨
- 기하학에 대한 풍부한 정보를 인코딩함

![](../assets/images/Pasted image 20260207030910.png)
마찬가지로, 라플라시안은 함수의 곡률(curvature)을 측정한다.

입력으로 스칼라 함수를 받는다.

보통 $\Delta$ 델타로 표기한다.

라플라시안의 많은 시작점들
- 그라디언트의 발산: $\Delta f:=\nabla\cdot\nabla f=div(gradf)$
    
- 2계 편미분의 합: $\Delta f:=\Sigma_{i=1}^{n}\partial^{2}f/\partial x_{i}^{2}$
    
- 디리클레 에너지의 그라디언트: $\Delta f:=-\nabla_{f}(\frac{1}{2}||\nabla f||^{2})$
    
- 유추에 의한 정의: 그래프 라플라시안
	- ![](../assets/images/Pasted image 20260207031033.png)
- 표면적의 변화(variation)

$$\Delta f:=\Sigma_{i}\partial^{2}f/\partial x_{i}^{2}$$
함수 $f$의 라플라시안은 함수의 모든 이차 편미분의 합이다.


### 헤시안 (Hessian)
![](../assets/images/Pasted image 20260207032253.png)
헤시안은 복잡한 함수를 몇 가지 간단한 항으로 국소적으로(locally) 근사하는 데 도움을 준다.

- 테일러 급수를 떠올려 보자.$$f(x)=f(x_{0})+f^{\prime}(x_{0})(x-x_{0})+\frac{(x-x_{0})^{2}}{2!}f^{\prime\prime}(x_{0})+\cdot\cdot\cdot$$
- **다변수 함수**에 대해서는 이것을 어떻게 할까 ?
    
- 이미 그라디언트를 사용한 최적 선형 근사에 대해 이야기했다.$$f(x)\approx f(x_{0})+\langle\nabla f(x_{0}),x-x_{0}\rangle$$
- 헤시안은 우리에게 그다음, **이차(quadratic)** 항을 제공한다.


일반적으로 기호 $\nabla^{2}$로 헤시안을 표기한다.

그라디언트가 **함수의 편미분을 제공하는 벡터**였던 것처럼, 헤시안은 **그라디언트의 편미분을 제공하는 연산자**이다.
$$(\nabla^{2}f)u:=D_{u}(\nabla f)$$

함수 $f(x):R^{n}\rightarrow R$에 대해, 더 명시적으로 쓸 수 있다.
$$\nabla^{2}f:=[\begin{matrix}\frac{\partial^{2}f}{\partial x_{1}\rho x_{1}}&...&\frac{\partial^{2}f}{\partial x_{1}\partial x_{a}}\\ .&...&.\\ .&...&.\\ \frac{\partial^{2}f}{\partial x_{i}\partial x_{1}}&...&\frac{\partial^{2}f}{\partial x_{i}\partial x_{i}}\end{matrix}]$$
[헤시안 행렬은 항상 대칭이다.]

헤시안을 사용하여, 이제 임의의 매끄러운 다변수 함수 $f(x)$의 $x_0$ 주변에서의 2차 근사를 쓸 수 있다.
$$f(x)\approx f(x_{0})+\langle\nabla f(x_{0}),x-x_{0}\rangle+\langle\nabla^{2}f(x_{0})(x-x_{0}),x-x_{0}\rangle/2$$

![](../assets/images/Pasted image 20260207032652.png)

