>This post is a personal study note based on CMU 15-462 (Computer Graphics) 2020/fall lectures.
>All original lecture slides and videos are copyrighted by the instructors.

강의에 대한 정보와 자료는 아래 홈페이지에서 확인할 수 있습니다.
https://15462.courses.cs.cmu.edu/fall2020/home

본 포스팅은 강의 내용을 바탕으로 하되, 이해를 돕기 위해 별도의 자료 조사와 개념 정리를 덧붙여 작성했습니다. 따라서 원 강의 내용에 추가적으로 개인적인 학습 자료가 다수 포함되어 있습니다.

---

### Keyframing
![](../assets/images/Pasted%20image%2020260316223638.png)

keyframing의 핵심은 모든 frame을 직접 만드는 것이 아니라 중요한 event만 지정하는 것. 컴퓨터는 그 사이를 보간이나 근사로 채운다.
여기서 event는 위치만 뜻하지 않는다. color, light intensity, camera zoom, orientation도 전부 대상.

즉 animation은 시간에 따라 바뀌는 attribute를 다루는 문제. 핵심 순간만 control로 주고, 중간 값은 계산으로 얻는다. 작업량은 줄지만, 어떤 방식으로 연결할지 선택하는 문제가 새로 생긴다.

### Spline Interpolation

키프레임 사이를 보간하는 방법.
수학에서는 구간별 다항식으로 이루어진 함수 전체를 spline이라 부른다.

주어진 knot $(t_i, f_i)$에 대해 spline은 $f(t_i)=f_i$를 만족하고, 각 구간 $[t_i, t_{i+1}]$에서는 $$f(t) \space =: \space \sum_{j=1}^{d} c_i t^j \space =: \space p_i(t)$$위의 다항식으로 표현된다. 점을 잇되 부드럽게 잇는 방식.

![](../assets/images/Pasted%20image%2020260316224841.png)
구간별 다항식 보간은 단순하지만 corner에서 속도가 갑자기 바뀐다. 이상적인 모델로 보면 knot에서 acceleration이 매우 거칠고, 경우에 따라 무한대로 튄다고 볼 수 있다. animation에서는 이런 급격한 변화가 어색하게 보이기 쉽다.

### Cubic and Constraints
![](../assets/images/Pasted%20image%2020260316230031.png)
실무와 수업에서 cubic spline이 자주 나오는 이유는 균형이 좋기 때문이다. 너무 단순하지도 않고, 너무 복잡하지도 않다. 높은 차수의 다항식은 룽게 현상처럼 구간 끝에서 쓸데없는 진동이 커질 수 있어 오히려 더 나빠질 수 있다.

하나의 cubic을 $p(t)=at^3+bt^2+ct+d$라 두면 자유도는 네 개. endpoint 조건 $p(0)=p_0$, $p(1)=p_1$만으로는 식이 두 개라서 해가 하나로 정해지지 않는다. 여기에 $p'(0)=u_0$, $p'(1)=u_1$를 더하면 조건이 네 개가 되고, 계수 $a,b,c,d$를 고유하게 정할 수 있다.

이 관점이 중요하다. spline을 맞춘다는 일은 결국 자유도와 제약 조건을 맞추는 선형대수 문제.
조건이 부족하면 해가 많고, 조건이 과하면 해가 없을 수 있다.

### Natural Spline
![](../assets/images/Pasted%20image%2020260316230454.png)
natural spline은 각 구간의 cubic이 keyframe을 정확히 지나가고, knot에서 1차 미분과 2차 미분이 이어지게 만든다. 즉 위치뿐 아니라 속도와 acceleration도 비교적 매끈하게 연결하는 방식.

natural spline의 조건은 $$p_i(t_i)=f_i$$$$p_i(t_{i+1})=f_{i+1}$$$$p_i'(t_{i+1})=p_{i+1}'(t_{i+1})$$$$p_i''(t_{i+1})=p_{i+1}''(t_{i+1})$$
으로 볼 수 있다. 양 끝에서는 $p''=0$으로 두어 남는 자유도를 정한다. 이 끝점 조건 때문에 natural이라는 이름이 붙는다.

장점은 interpolation과 smoothness. 단점은 local control이 약하다는 점이다. 한 점을 움직이면 큰 선형 시스템 전체가 다시 바뀌므로 curve 전반이 함께 흔들릴 수 있다.

### Hermite and Bezier

![](../assets/images/Pasted%20image%2020260316230507.png)
Hermite spline은 각 구간을 endpoint와 tangent로 정한다. Bezier form은 같은 정보를 네 개의 control point로 보는 방식. 둘은 표현이 다를 뿐, 기본적으로 같은 cubic 조각을 다룬다.

인접 구간이 같은 tangent를 공유하면 $C^1$ continuity까지는 쉽게 얻는다. 이 방식의 강점은 local control. 한 구간은 그 구간의 endpoint와 tangent만으로 정해지므로 편집이 직관적이다. 대신 자동으로 $C^2$ continuity가 보장되지는 않는다. 즉 interpolation과 locality는 좋지만, acceleration까지 항상 매끈하다고 보기는 어렵다.

2D vector art에서 Bezier가 널리 쓰이는 이유도 여기에 있다. 사용자가 curve 모양을 직접 당기고 밀기 쉽다. animation에서도 artist control이 중요한 경우에 잘 맞는다.

### Catmull-Rom
![](../assets/images/Pasted%20image%2020260316230944.png)
Catmull-Rom spline은 tangent를 직접 주기 번거로울 때 쓰는 Hermite 계열의 기본 선택지다.
핵심 아이디어는 이웃한 점의 차이로 tangent를 자동 추정하는 것.

**대표적인 형태** $$u_i=\dfrac{f_{i+1}-f_{i-1}}{t_{i+1}-t_{i-1}}$$
사용자는 점들만 주면 되고, curve는 그 정보를 바탕으로 자연스러운 tangent를 만든다.
camera path나 기본 motion 보간에서 자주 쓰이는 이유.

성질은 Hermite와 비슷하다. interpolation은 유지하고, local control도 좋다.
대신 $C^2$ continuity를 보장하는 방식은 아니다.

### B-Spline

![](../assets/images/Pasted%20image%2020260316231436.png)
key point를 정확히 지나가는 성질을 일부 포기하고, continuity와 locality를 더 강하게 챙기는 쪽. curve가 control point를 따라가기는 하지만 꼭 통과하지는 않는다.

구현 관점에서는 basis function을 재귀적으로 쌓고, curve를 그 basis의 선형 결합으로 표현한다. 그래서 편집할 때 유리하다.
한 control point를 움직여도 curve 전체가 크게 흔들리지 않는다.
cubic B-spline은 보통 $C^2$ continuity도 좋다. 대신 keyframe을 정확히 맞춰야 하는 상황이라면 그대로 쓰기엔 불편할 수 있다.

B-spline은 interpolation 대신 approximation을 택한 경우. modeling과 path design에서 자주 쓰이는 이유가 충분함.

### Spline Trade-Offs

cubic spline 계열에서는 세 성질을 동시에 완벽하게 잡기 어렵다. interpolation, $C^2$ continuity, locality 사이의 상충 관계가 생긴다.

| Spline | Interpolation | $C^2$ continuity | Locality | 핵심 |
| --- | --- | --- | --- | --- |
| Natural | O | O | X | 매우 매끈하지만 전역적으로 묶임 |
| Hermite 또는 Bézier | O | X | O | 제어가 직관적이고 local |
| Catmull-Rom | O | X | O | tangent 자동 추정 |
| B-Spline | X | O | O | 부드럽고 local, 대신 점을 꼭 지나가지는 않음 |

무엇이 가장 좋은 spline인지는 작업 목적에 따라 달라진다. exact keyframe 통과가 중요한지, acceleration의 smoothness가 중요한지, 아니면 local editing이 중요한지. 이 선택이 animation 품질과 작업 흐름을 함께 바꾼다.

### Camera Path

spline으로 보간하는 값은 위치만이 아니다. camera animation에서는 position, viewing direction, up direction이 함께 변한다. 위치만 예로 들면 $f(t)=(x(t), y(t), z(t))$이고, 각 성분 $x(t), y(t), z(t)$ 자체가 spline이다.

즉 camera move는 3차원 경로 하나만의 문제가 아니다. 어디에 있는지, 어디를 보는지, 화면의 위쪽을 어떻게 잡는지까지 포함한 시간 함수 묶음. 그래서 부드러운 camera motion을 만들려면 여러 attribute를 함께 보간해야 한다.

### Character Control
![](../assets/images/Pasted%20image%2020260316231818.png)

character animation에서는 scene graph 또는 kinematic chain으로 몸을 표현한다. 팔, 다리, 몸통이 tree 형태의 transformation으로 연결된 구조. animation은 결국 이 transformation들의 parameter를 시간에 따라 바꾸는 일이다.

문제는 자유도가 많다는 점. rig가 복잡해질수록 animator가 직접 모든 angle을 잡는 일은 금방 버거워진다. 그래서 inverse kinematics가 중요해진다. 손이 책상을 잡게 하라, 발이 바닥에 닿게 하라 같은 goal을 주고, 알고리즘이 나머지 joint 값을 찾아내는 방식.

강의에서는 이 과정을 수치 최적화의 문제로 본다. 원하는 목표와 현재 상태의 차이를 줄이는 방향으로 parameter를 갱신하는 흐름. animation과 robotics가 만나는 지점이기도 하다.

### Surface Deformation
![](../assets/images/Pasted%20image%2020260316232033.png)
skeletal animation은 내부의 rigid skeleton이 바깥 mesh를 움직이는 방식이다. 각 bone은 vertex마다 다른 영향력을 가지며, 기본형에서는 여러 bone transform의 결과를 weight 평균해서 최종 위치를 만든다. 가장 단순한 형태가 linear blend skinning.

이 방식 덕분에 cube man 같은 분절된 물체가 아니라, 연속적인 피부와 근육처럼 보이는 deformation을 만들 수 있다. 다만 실제 몸처럼 volume을 잘 유지하고 자연스럽게 구부러지게 만드는 일은 여전히 활발한 연구 주제.

blend shapes는 skeleton 대신 여러 표정이나 pose mesh를 직접 섞는 방법. 예를 들어 happy, sad, sleepy 같은 얼굴 shape을 미리 만들고, 시간에 따라 weight를 바꿔 최종 얼굴을 만든다. 여기서 spline은 각 weight가 시간에 따라 어떻게 변할지 정하는 control 역할.
