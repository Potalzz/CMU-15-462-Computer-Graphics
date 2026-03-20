>This post is a personal study note based on CMU 15-462 (Computer Graphics) 2020/fall lectures.
>All original lecture slides and videos are copyrighted by the instructors.

강의에 대한 정보와 자료는 아래 홈페이지에서 확인할 수 있습니다.
https://15462.courses.cs.cmu.edu/fall2020/home

본 포스팅은 강의 내용을 바탕으로 하되, 이해를 돕기 위해 별도의 자료 조사와 개념 정리를 덧붙여 작성했습니다. 따라서 원 강의 내용에 추가적으로 개인적인 학습 자료가 다수 포함되어 있습니다.

---

## Core Quantities

| Quantity               | Meaning                       | Formula                                 | Unit          |
| ---------------------- | ----------------------------- | --------------------------------------- | ------------- |
| Radiant energy $Q$     | 전체 energy                     | $Q$                                     | $J$           |
| Radiant energy density | area당 energy                  | $\frac{dQ}{dA}$                         | $J/m^2$       |
| Radiant flux $\Phi$    | time당 energy                  | $\frac{dQ}{dt}$                         | $W$           |
| Irradiance $E$         | area당 flux                    | $\frac{d\Phi}{dA}$                      | $W/m^2$       |
| Radiance $L$           | area, direction당 flux         | $\frac{d^2\Phi}{dA\,d\omega\cos\theta}$ | $W/(m^2\,sr)$ |
| Radiant intensity $I$  | point source의 direction당 flux | $\frac{d\Phi}{d\omega}$                 | $W/sr$        |

- 전체 energy를 time으로 나누면 flux, flux를 area로 나누면 irradiance, irradince에서 특정 방향과 묶여 있는 빛의 세기가 **radiance**
  들어오는 빛은 incoming radiance, 나가는 빛은 outgoing radiance.
- 이미지 생성은 결국 pixel마다 irradiance를 추정하는 일.

## Spectrum
![](../assets/images/Pasted%20image%2020260310024142.png)
- photon 1개의 energy는 $Q = \frac{hc}{\lambda}$.
- wavelength가 짧을수록 photon energy가 큼. 보통 blue 쪽이 더 큼.
- Color까지 다루려면 wavelength 축으로도 나눠야 함.
- Spectral power distribution은 wavelength당 irradiance.
	- ![](../assets/images/Pasted%20image%2020260310024358.png)
- Spectral radiance는 위치, 방향, wavelength까지 포함한 light 설명. 가장 완전한 표현에 가까움.

## Geometry
![](../assets/images/Pasted%20image%2020260310024630.png)
- 같은 flux라도 surface가 기울면 더 넓은 area에 퍼짐.
- Projected area 관계는 $A = A_0 \cos\theta$.
- 그래서 irradiance는 $\cos\theta$에 비례. Lambert 법칙의 핵심.
- 가장 단순한 surface shading 처리 방법은 단위 법선과 단위 광원 방향의 내적을 구하면 됨. $\max(0, dot(N, L))$
	- ![](../assets/images/Pasted%20image%2020260310024831.png)
	- 음수 clamp가 필요한 이유는 surface 뒤쪽에서 오는 light를 그대로 쓰면 물리적으로 맞지 않기 때문.

## Distance and Sources

- directional light는 매우 멀리 있는 광원 모델. scene 전체에서 빛의 방향이 거의 같음.
- isotropic point source는 모든 direction으로 균일하게 방출하는 점광원.
	- ![](../assets/images/Pasted%20image%2020260310025432.png)
- 점광원에서 거리 $r$의 irradiance는 $E(r)=\frac{\Phi}{4\pi r^2}$.
- 그래서 밝기는 거리 제곱에 반비례
	- 광원의 거리가 멀어질 수록 밝기는 거리 $r$에 대해 비율로 $1/r^2$만큼 변함.
	- (e.g 0.1m -> 0.2m로 멀어지면 4배 어두워짐)

## Solid Angle
![](../assets/images/Pasted%20image%2020260310025604.png)
- angle이 원 위 호를 기준으로 한 양이라면 solid angle은 구 위 area를 기준으로 한 양.
- $\theta = \frac{l}{r}$, $\Omega = \frac{A}{r^2}$.
- sphere 전체 solid angle은 $4\pi$ steradian.
- Differential solid angle은 $d\omega = \sin\theta\, d\theta\, d\phi$.
	- ![](../assets/images/Pasted%20image%2020260310025658.png)
- Radiance와 environment lighting 적분에서 계속 등장.

## Radiance

- Radiance $L(p,\omega)$는 한 점에서 한 방향으로 흐르는 light의 밀도.
- Ray 하나에 붙는 대표 quantity. Rendering의 중심.
- 진공에서는 radiance가 ray를 따라 일정.
- Incident radiance $L_i$와 exitant radiance $L_o$는 다를 수 있음.
	- ![](../assets/images/Pasted%20image%2020260310025823.png)
- Pinhole camera는 사실상 radiance를 측정하는 장치.

## Light Field

- Light field는 ray마다 radiance를 붙인 함수.
- Camera는 light field의 작은 일부를 샘플링.

## Illumination
![](../assets/images/Pasted%20image%2020260310025919.png)
- Surface의 irradiance는 위 hemisphere에서 들어오는 radiance를 모두 합친 값.
- 식은 $E(p)=\int_{H^2} L_i(p,\omega)\cos\theta\, d\omega$.
- 균일한 hemispherical source라면 occlusion이 없을 때 $E(p)=L\pi$.
- 실제 scene에서는 geometry가 light를 가리므로 점마다 값이 달라짐.
- 이 생각을 단순화한 기법이 ambient occlusion.
- AO는 self-shadowing 정도를 미리 계산해 texture로 bake하거나 screen space에서 근사.
	- ![](../assets/images/Pasted%20image%2020260310030000.png)

## Area Sources
![](../assets/images/Pasted%20image%2020260310030023.png)
- 실제 광원은 point보다 area를 가지는 경우가 많음.
- Uniform area source는 projected solid angle이 클수록 더 큰 irradiance를 줌.
- 핵심 식은 $E(p)=L\Omega^\perp$.
- Disk source 예시에서는 $\Omega^\perp = \pi \sin^2 \alpha
	- ![](../assets/images/Pasted%20image%2020260310030116.png)
- Area light는 shadow가 더 부드럽고, 실제 조명과도 더 비슷.

## Photometry
![](../assets/images/Pasted%20image%2020260310030150.png)
- Radiometry가 physics 중심이라면 photometry는 human vision 중심.
- Luminance $Y$는 radiance를 눈의 민감도 곡선 $V(\lambda)$로 가중 적분한 값.
- 식은 $Y(p,\omega)=\int_0^\infty L(p,\omega,\lambda)V(\lambda)\, d\lambda$.
- 즉 사람에게 어떻게 보이는지를 반영한 양.
- Radiometric quantity와 photometric quantity는 대응 관계가 있음.

## 요약

- Radiometry는 light를 total energy에서 시작해 time, area, direction, wavelength로 세분화하는 언어 체계.
- Image generation에서는 irradiance와 radiance가 특히 중요.
- Irradiance는 surface가 받는 양, radiance는 ray를 따라 전달되는 양.
- Rendering은 결국 scene 안의 radiance를 계산하고, 그것을 camera sensor 값으로 모으는 과정.
- 다음 단계는 material과 scattering. light가 surface를 만나며 어떻게 바뀌는지 이해하는 일.
