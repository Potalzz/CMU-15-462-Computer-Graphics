>This post is a personal study note based on CMU 15-462 (Computer Graphics) 2020/fall lectures.
>All original lecture slides and videos are copyrighted by the instructors.

강의에 대한 정보와 자료는 아래 홈페이지에서 확인할 수 있습니다.
https://15462.courses.cs.cmu.edu/fall2020/home

본 포스팅은 강의 내용을 바탕으로 하되, 이해를 돕기 위해 별도의 자료 조사와 개념 정리를 덧붙여 작성했습니다. 따라서 원 강의 내용에 추가적으로 개인적인 학습 자료가 다수 포함되어 있습니다.

---

## Overview

- Monte Carlo rendering은 photorealistic image를 만들기 위한 ray tracing 기반 방법
- 입력은 camera, geometry, materials, lights. 출력은 image
- 핵심 과제는 각 픽셀에 들어오는 빛의 양을 계산하는 것
- 이 과정에서 color, material, radiometry, numerical integration, geometric query, spatial data structure, rendering equation이 함께 묶인다
- sample 하나를 단순한 점 하나로 보기보다, light가 scene을 지나 camera에 도달하는 path 하나로 보는 편이 정확함.

## Ray Tracing vs Rasterization
![](../assets/images/Pasted%20image%2020260315194134.png)
- 두 방법 모두 최종적으로는 image를 만든다. 차이는 계산 순서와 조명 표현력
- rasterization은 primitive 중심. 각 triangle이 어떤 sample을 덮는지 계산하고, 가시성은 Z-buffer로 해결
- ray tracing은 sample 중심. 각 pixel이나 sample에서 ray를 쏘고, scene과의 교차를 본다. 가시성 가속은 BVH 같은 spatial data structure
- rasterization은 한 primitive씩 처리하므로 다른 물체 때문에 생기는 shadow, reflection, refraction, indirect illumination을 다루기 까다롭다

![](../assets/images/Pasted%20image%2020260315195134.png)
- ray tracing은 한 ray가 만나는 모든 물체와 관계를 따라갈 수 있어 global illumination에 강함

## Rendering Equation

렌더링의 중심 식은 다음과 같다
$$L_o(p,\omega_o)=L_e(p,\omega_o)+\int_{H^2} f_r(p,\omega_i\to\omega_o)L_i(p,\omega_i)\cos\theta_i\,d\omega_i$$
- $L_o$: 점 $p$에서 방향 $\omega_o$로 나가는 빛
- $L_e$: 점 $p$가 스스로 내는 빛
- $f_r$: 들어온 빛이 어떤 방향으로 얼마나 반사되는지 나타내는 재질 함수. BRDF
- $L_i$: 방향 $\omega_i$에서 들어오는 빛
- $\cos\theta_i$: 비스듬히 들어오는 빛의 기여가 줄어드는 항
- 결국 픽셀 색은 이 적분을 얼마나 잘 추정하느냐의 문제

## Monte Carlo Basics

- Monte Carlo integration의 기본은 무작위 sample의 평균으로 적분값을 추정하는 것
- 강의 맥락에서는 sample 하나가 light path 하나
- expected value는 평균적으로 어떤 값이 나오는가
- variance는 sample이 평균 주변에서 얼마나 흔들리는가
- 서로 독립이고 같은 분포를 따르는 sample $Y_1,\dots,Y_N$을 평균내면 $V\left[\frac{1}{N}\sum_{i=1}^{N}Y_i\right]=\frac{1}{N}V[Y]$
- sample 수를 늘리면 노이즈는 줄지만, 감소 속도는 느리다. 분산이 $1/N$ 비율로 내려가기 때문
- 대수의 법칙 덕분에 조명, 재질, 기하가 복잡해도 **sample을 충분히 늘리면 정답 이미지에 수렴한다.**

## Biasing and Importance Sampling

- sample을 uniform하게만 뽑을 필요는 없다
- 어떤 분포 $p(x)$로 sample을 뽑아도 보정만 제대로 하면 평균적으로 치우치지 않는 추정식을 만들 수 있다
- $\int_{\Omega} f(x)\,dx \approx \frac{1}{N}\sum_{i=1}^{N}\frac{f(X_i)}{p(X_i)}$
- 나누는 이유는 자주 뽑히는 위치의 기여를 줄이고, 드물게 뽑히는 위치의 기여를 키우기 위해서
- 중요하지 않은 영역에 sample을 많이 쓰면 계산만 늘고 정보는 적다
- importance sampling의 목표는 적분 함수가 큰 곳, 즉 실제 기여가 큰 곳에 sample을 더 배치하는 것
- ![](../assets/images/Pasted%20image%2020260315195422.png)

## Direct Lighting
![](../assets/images/Pasted%20image%2020260315195606.png)
- 먼저 다루는 적분은 한 점에 직접 들어오는 빛, direct lighting
- irradiance는 $E(p)=\int_{\Omega}L(p,\omega)\cos\theta\,d\omega$
- 가장 단순한 방법은 hemisphere 전체에서 방향을 uniform하게 뽑는 것
- 이때 $p(\omega)=\frac{1}{2\pi}$ 이고 추정식은 $F_N=\frac{2\pi}{N}\sum_{i=1}^{N}L(p,\omega_i)\cos\theta_i$
- 실제 구현에서는 visibility를 같이 본다. ray가 light에 가기 전에 다른 물체를 맞으면 그 방향의 기여는 0
- 문제는 대부분의 방향이 light를 맞히지 못한다는 점. 많은 sample이 거의 아무 일도 하지 않음
- 그래서 픽셀마다 결과가 들쭉날쭉하고 image에 noise가 남는다
- 방향 생성은 rejection sampling이나 inversion method로 구현 가능
	- ![](../assets/images/Pasted%20image%2020260315195526.png)

## Light Sampling
![](../assets/images/Pasted%20image%2020260315195634.png)
- direct lighting에서는 hemisphere 전체보다 light source 자체를 sample하는 편이 훨씬 효율적인 경우가 많다
- 방향 적분을 light area 적분으로 바꾸면
- $$E(p)=\int_{A'}L_o(p',\omega')V(p,p')\frac{\cos\theta\cos\theta'}{\|p-p'\|^2}\,dA'$$
- $V(p,p')$는 visibility function. 보이면 1, 가려지면 0
- light의 면적을 균일하게 sample하면 $p(p')=\frac{1}{A'}$
- 추정식은$$F_N=\frac{A'}{N}\sum_{i=1}^{N}L_o(p_i',\omega_i')V(p,p_i')\frac{\cos\theta_i\cos\theta_i'}{\|p-p_i'\|^2}$$
- 이 방식은 light를 향하는 ray를 바로 뽑으므로 헛되는 sample이 크게 줄어든다
- 같은 100 sample이어도 hemisphere uniform sampling보다 결과가 훨씬 매끈
- ![](../assets/images/Pasted%20image%2020260315195842.png)![](../assets/images/Pasted%20image%2020260315195752.png)

## Cosine-Weighted Sampling

- diffuse 표면의 irradiance 적분에는 $\cos\theta$가 직접 들어간다
- 표면 normal 근처 방향은 중요하고, grazing angle은 덜 중요
- 그래서 $p(\omega)=\frac{\cos\theta}{\pi}$ 같은 cosine-weighted sampling을 자주 쓴다
- $f(\omega)=L_i(\omega)\cos\theta$라면 추정식은 $F_N=\frac{\pi}{N}\sum_{i=1}^{N}L_i(\omega_i)$ 꼴로 정리된다
- 핵심은 기여가 큰 방향에 ray를 몰아 분산을 줄이는 것

## Path Tracing
![](../assets/images/Pasted%20image%2020260315200024.png)
- direct lighting만 계산하면 light가 한 번만 튄 경우만 반영된다
- 실제 scene에서는 빛이 벽, 천장, 바닥, 유리에서 여러 번 튄다. indirect illumination
- path tracing은 incoming direction $\omega_i$를 하나 sample하고, 그 방향으로 ray를 쏜 뒤, 다음 hit point에서 같은 계산을 재귀적으로 반복하는 방식
- rendering equation의 재귀 구조를 그대로 알고리즘으로 옮긴 셈
- 한 sample은 path 하나. 많은 path를 평균내면 픽셀 색이 된다
- 간단히 쓰면 indirect term의 한 sample은 $$\frac{f_r(p,\omega_i\to\omega_o)L_o(p',-\omega_i)\cos\theta_i}{p(\omega_i)}$$
- 여기서 $p'$는 ray가 다음에 만나는 점
- bounce 수가 늘수록 shadow 안쪽이 밝아지고, glass 같은 재질의 모습도 더 제대로 드러난다
- ![](../assets/images/Pasted%20image%2020260315200105.png)![](../assets/images/Pasted%20image%2020260315200133.png)![](../assets/images/Pasted%20image%2020260315200157.png)

## Russian Roulette

- path tracing은 재귀가 길어질 수 있다. 언젠가는 멈춰야 한다
- bounce 수를 고정해서 자르는 방법도 있지만, 긴 path의 기여를 놓치기 쉽다
- Russian roulette은 path를 확률적으로 종료하는 방법
- 기여가 작아 보이는 path는 일정 확률로 끊고, 살아남은 path는 그만큼 크게 보정한다
- 살아남을 확률을 $p_{rr}$라 하면, 살아남은 sample contribution을 $\frac{X}{p_{rr}}$로 두고 종료된 sample은 0으로 둔다
- 기대값은 $$p_{rr}\frac{X}{p_{rr}}+(1-p_{rr})0=X$$
- 그래서 평균적으로 치우치지 않는다. 단순히 작은 값을 그냥 버리는 것과 다르다
- 장점은 속도 향상
- 단점은 너무 공격적으로 끊으면 noise 증가
- ![](../assets/images/Pasted%20image%2020260315200326.png)![](../assets/images/Pasted%20image%2020260315200343.png)


## Efficiency

- 추정식을 볼 때는 variance만 보면 부족하다. cost도 같이 봐야 한다
- 강의에서는 효율을 대략 $$\text{Efficiency}\propto\frac{1}{\text{Variance}\times\text{Cost}}$$
- 분산이 2배면 같은 품질을 얻으려면 sample도 대략 2배 필요
- 비용이 2배면 같은 품질을 얻는 시간도 대략 2배
- 좋은 renderer는 적은 ray로 작은 variance를 내는 sampling 전략을 고른다

## Algorithm Sketch

- 각 pixel에서 camera ray 생성
- ray와 scene의 교차를 BVH 등으로 빠르게 찾기
- hit point가 emitter면 emission 반영
- direct lighting은 light sampling으로 추정
- indirect lighting은 새 방향을 sample해서 path tracing 재귀 호출
- 중간에 Russian roulette로 종료 여부 결정
- 한 픽셀에서 여러 path 결과를 평균내어 최종 color 계산

## 요약

- Monte Carlo rendering은 rendering equation을 확률적으로 푸는 방법
- sample을 많이 쓰면 정답에 수렴하지만, 그냥 많이만 쓰면 느리다
- 그래서 importance sampling, light sampling, cosine-weighted sampling, Russian roulette이 핵심
- ray tracing이 realistic shadow, reflection, refraction, indirect illumination을 잘 다루는 이유는 light path 자체를 따라가기 때문
- 결국 품질과 속도는 sample을 얼마나 똑똑하게 쓰느냐에 달려 있다
