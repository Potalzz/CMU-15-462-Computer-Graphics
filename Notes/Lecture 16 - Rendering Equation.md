>This post is a personal study note based on CMU 15-462 (Computer Graphics) 2020/fall lectures.
>All original lecture slides and videos are copyrighted by the instructors.

강의에 대한 정보와 자료는 아래 홈페이지에서 확인할 수 있습니다.
https://15462.courses.cs.cmu.edu/fall2020/home

본 포스팅은 강의 내용을 바탕으로 하되, 이해를 돕기 위해 별도의 자료 조사와 개념 정리를 덧붙여 작성했습니다. 따라서 원 강의 내용에 추가적으로 개인적인 학습 자료가 다수 포함되어 있습니다.

---

## Recap

- Radiance는 한 점과 한 방향에서의 빛의 세기. 위치만이 아니라 방향 정보가 핵심
- Incident radiance와 exitant radiance는 같은 물리량을 들어오는 쪽, 나가는 쪽으로 나눠 부르는 표현
- Irradiance는 여러 방향에서 들어오는 radiance를 합친 값. $E = \int_{H^2} L(\omega)\cos\theta\,d\omega$
- Radiance의 정의는 $L = \dfrac{\partial^2\Phi}{\partial\Omega\,\partial A\cos\theta}$  
  여기서 $\partial A\cos\theta$는 projected area
	- ![](../assets/images/Pasted%20image%2020260311111954.png)
- 헷갈리기 쉬운 점 하나. $\cos\theta$가 두 이유로 등장하는데, 여기서 사용되는 값은 구를 매개변수화 하는 방식과 관련이 있음
  하나는 Lambert's law, 다른 하나는 구면 적분의 parameterization
	- ![](../assets/images/Pasted%20image%2020260311111922.png)


## Rendering Equation

- 목표는 점 $p$에서 방향 $\omega_o$로 나가는 radiance $L_o$(irradiance에서 특정 방향으로 나가는 빛의 세기인 outgoing radiance)를 구하는 것
- 핵심 식은 아래와 같음  
  $L_o(p,\omega_o) = L_e(p,\omega_o) + \int_{H^2} f_r(p,\omega_i \rightarrow \omega_o)L_i(p,\omega_i)\cos\theta_i\,d\omega_i$
	- ![](../assets/images/Pasted%20image%2020260311112634.png)
- $L_e$는 스스로 내는 빛, $f_r$는 surface가 빛을 어떻게 튕기는지, $L_i$는 들어오는 빛
- 어려운 이유는 재귀성. $L_i$를 구하려면 다음 점에서 다시 같은 문제를 풀어야 함
	- ![](../assets/images/Pasted%20image%2020260311112612.png)
- 재귀의 base case는 emission. ray가 light source에 닿으면 $L_e$가 직접 기여
- 그래서 renderer는 light를 앞에서 뒤로 쫓기보다 camera에서 ray를 뒤로 쏘는 방식이 자연스럽다
	- ![](../assets/images/Pasted%20image%2020260311112813.png)

## Reflection Models

- 반사는 빛이 surface에 닿았다가 같은 쪽으로 나오는 과정
- 어떤 $f_r$를 쓰느냐가 material appearance를 결정
- 대표적인 유형
  - ![](../assets/images/Pasted%20image%2020260311113458.png)
  - diffuse: 여러 방향으로 고르게 퍼짐
  - ideal specular: 거울처럼 한 방향으로 반사
  - glossy specular: 거울 반사 주변으로 퍼짐
  - retro-reflective: 들어온 쪽으로 다시 돌아감
- 같은 incident radiance라도 material에 따라 exitant radiance가 완전히 달라진다
	- ![](../assets/images/Pasted%20image%2020260311113602.png)
	- ![](../assets/images/Pasted%20image%2020260311113615.png)

## Scattering off a surface: BRDF

- BRDF는 $f_r(\omega_i \rightarrow \omega_o)$
- 들어온 빛이 표면에서 어떤 방향으로 얼마나 반사되어 나가는지를 타나내는 함수(incoming direction 하나가 outgoing direction마다 얼마나 퍼지는지 기록)
- 꼭 지켜야 할 성질
  - 음수가 아니어야 함
  - 에너지 보존: $\int_{H^2} f_r(\omega_i \rightarrow \omega_o)\cos\theta_i\,d\omega_i \le 1$
  - reciprocity: $f_r(\omega_i \rightarrow \omega_o) = f_r(\omega_o \rightarrow \omega_i)$
- Lambertian surface는 가장 단순한 diffuse model(빛이 모든 출력 방향으로 동일한 확률로 반사된다고 가정)
  $f_r = \frac{\rho}{\pi}$, 따라서 $L_o = \frac{\rho}{\pi}E$
	- ![](../assets/images/Pasted%20image%2020260311114000.png)
- 거울 반사는 일반 함수라기보다 한 방향에 집중된 분포에 가깝다
- reflected direction은 $\omega_o = -\omega_i + 2(\omega_i \cdot n)n$
- BRDF의 단위는 $1/\mathrm{sr}$

## Transmission

- 어떤 material은 반사만이 아니라 transmission도 일어난다
- 굴절은 매질이 바뀔 때 방향이 꺾이는 현상
- 스넬의 법칙: $\eta_i \sin\theta_i = \eta_t \sin\theta_t$
	- ![](../assets/images/Pasted%20image%2020260311114313.png)
- 더 조밀한 매질에서 덜 조밀한 매질로 나갈 때 angle이 크면 total internal reflection이 생길 수 있다
- 즉, 나가지 못하고 다시 반사
- 실제 재질에서 angle이 커질수록 reflectance가 커지는 경우가 많다. 이것이 Fresnel effect
	- ![](../assets/images/Pasted%20image%2020260311114221.png)
- 물 표면이 멀리서는 거울처럼 보이고, 아래를 내려다볼 때는 내부가 비치는 이유
- ![](../assets/images/Pasted%20image%2020260311114401.png)
- ![](../assets/images/Pasted%20image%2020260311114416.png)

## Advanced Materials

- anisotropic reflection은 회전 방향에 따라 반사 모양이 달라지는 경우. brushed metal이 대표적
- translucent material은 빛이 표면 안으로 들어가 내부에서 퍼진 뒤 다른 위치로 나오는 경우가 많다
- 피부, 옥, 잎 같은 재질이 여기에 가까움
- 이때는 한 점에서 들어와 같은 점에서 나간다는 BRDF 가정이 깨진다
- 그래서 BSSRDF 같은 더 넓은 모델이 필요
- 일반화된 형태는 아래와 같음  
  $L(x_o,\omega_o) = \int_A \int_{H^2} S(x_i,\omega_i,x_o,\omega_o)L_i(x_i,\omega_i)\cos\theta_i\,d\omega_i\,dA$
- ![](../assets/images/Pasted%20image%2020260311114514.png)

## Monte Carlo Estimation

- reflection equation만 떼어 보면  
  $L_r(p,\omega_r) = \int_{H^2} f_r(p,\omega_i \rightarrow \omega_r)L_i(p,\omega_i)\cos\theta_i\,d\omega_i
	- ![](../assets/images/Pasted%20image%2020260311114626.png)
- 이 적분은 닫힌형으로 풀기 어렵기 때문에 Monte Carlo integration을 쓴다
- 방향 $\omega_j$를 랜덤으로 뽑고 평균을 내는 방식
- estimator는 아래와 같음  
  $\hat{L}_r = \dfrac{1}{N}\sum_{j=1}^{N}\dfrac{f_r(p,\omega_j \rightarrow \omega_r)L_i(p,\omega_j)\cos\theta_j}{p(\omega_j)}$
- $p(\omega)$를 BRDF나 incoming light와 비슷하게 잡으면 노이즈를 줄이기 쉬움
- 가장 중요한 부분은 $L_i$ 계산. 샘플 direction마다 ray를 다시 trace해서 재귀적으로 구함
	- ![](../assets/images/Pasted%20image%2020260311114722.png)

## Path Tracing

- 전체 rendering equation은 emitted light와 reflected light를 합친 형태
	- ![](../assets/images/Pasted%20image%2020260311114812.png)
- 실전에서는 direct illumination과 indirect illumination으로 나눠 계산
- overview
  1. 렌더링 방정식을 직접 조명과 간접 조명으로 분할
  2. 각 분할을 몬테카를로로 따로 추정
     - 각각에 대해 한 샘플
     - 가정: 픽셀 당 수백 개의 샘플
  3. 러시안 룰렛으로 경로 종료
- camera에서 ray를 쏘고, surface를 만날 때마다 새 direction을 sample하면서 path를 만든다
- path가 light source에 닿으면 기여를 더하고, 너무 길어지면 Russian roulette로 확률적으로 종료
- sample 수가 많을수록 결과가 안정
- indirect light까지 포함되면 반사, 굴절, color bleeding, 부드러운 global illumination이 훨씬 자연스럽게 보인

## Takeaways

- photorealistic rendering의 핵심 단위는 radiance
- rendering equation은 빛의 방출과 산란을 한 식으로 묶은 재귀 모델
- 재질의 차이는 대부분 scattering function 차이에서 나온다
- 현실적인 이미지는 BRDF, transmission, Fresnel, subsurface scattering 같은 모델을 잘 고르고, Monte Carlo와 path tracing으로 적분을 잘 추정할 때 얻어진다
- 결국 renderer는 각 픽셀에서 scene으로 나가는 ray를 따라가며 빛의 경로를 추정하는 계산기