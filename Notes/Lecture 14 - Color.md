>This post is a personal study note based on CMU 15-462 (Computer Graphics) 2020/fall lectures.
>All original lecture slides and videos are copyrighted by the instructors.

강의에 대한 정보와 자료는 아래 홈페이지에서 확인할 수 있습니다.
https://15462.courses.cs.cmu.edu/fall2020/home

본 포스팅은 강의 내용을 바탕으로 하되, 이해를 돕기 위해 별도의 자료 조사와 개념 정리를 덧붙여 작성했습니다. 따라서 원 강의 내용에 추가적으로 개인적인 학습 자료가 다수 포함되어 있습니다.

---

## Spectrum First
색을 **스펙트럼 기반**으로 생각하면 여러 이슈가 훨씬 명확해짐.
반대로 색을 근사적인 디지털 부호화(RGB, CMYK 등)로만 생각하면, 어떤 현상들은 도저히 설명/이해할 수 없음.

- 물리적으로 color는 빛의 frequency와 연결된다. wavelength가 길수록 frequency는 낮아지는 관계
- 사람이 볼 수 있는 영역은 visible spectrum뿐. 전체 electromagnetic spectrum에서 매우 좁은 구간
- white light는 하나의 color가 아니라 여러 visible wavelength가 함께 섞인 상태
- color를 가장 근본적으로 설명하는 방식은 **spectrum**. 즉 wavelength별로 얼마나 방출하거나 흡수하는지의 함수
- emission spectrum은 광원이 각 wavelength에서 얼마나 빛을 내는지
- absorption spectrum은 물체가 각 wavelength를 얼마나 흡수하는지
- chlorophyll은 red와 blue를 많이 흡수하고 green은 덜 흡수해서 식물이 green으로 보임
- RGB, CMYK 같은 표현은 편리한 요약일 뿐. 출발점은 spectrum
![](../assets/images/Pasted%20image%2020260307234313.png)

## Light and Surface

- 실제로 보이는 color는 광원 spectrum과 표면의 reflection 특성이 함께 만든 결과
- 광원의 emission spectrum을 $f(\nu)$, 표면 reflection spectrum을 $g(\nu)$라고 두면, 반사된 빛은 대략 $I(\nu)=f(\nu)g(\nu)$
- 같은 물체도 illumination이 달라지면 다른 색처럼 보일 수 있다
- color reproduction이 어려운 이유도 여기. 원하는 appearance를 얻으려면 물체 색만이 아니라 어떤 빛 아래에서 보일지도 알아야 함
![](../assets/images/Pasted%20image%2020260307234416.png)

## Human Vision

- 눈은 camera와 비슷한 optical system. 빛이 들어오고, retina에서 감지되고, signal이 brain으로 전달된다
- 막대 세포는 어두운 환경에서 주로 밝기를 감지한다
- 원뿔 세포는 밝은 환경에서 color를 감지한다. 대략 3종류의 원뿔 세포가 있으며 서로 다른 wavelength에 반응
- 막대 세포가 훨씬 많기 때문에 사람은 color 차이보다 brightness 차이에 더 민감한 편
- color vision은 화면 중심부에서 더 강하다. 중심와에 막대세포 density가 높기 때문
- cone response도 같은 방식. $S=\int \Phi(\lambda)S(\lambda)\,d\lambda$, $M=\int \Phi(\lambda)M(\lambda)\,d\lambda$, $L=\int \Phi(\lambda)L(\lambda)\,d\lambda$
- 중요한 점은 눈이 전체 spectrum을 그대로 보내지 않는다는 것. 최종적으로는 세 종류의 cone response로 많이 압축된 정보만 전달

![](../assets/images/Pasted%20image%2020260307234850.png)

## Metamers and Perception

- 서로 다른 두 spectrum이 같은 $S,M,L$ response를 만들 수 있다. 이를 metamer라 한다
- metamer가 있기 때문에 monitor나 printer가 원래 scene의 spectrum을 완벽히 복제하지 않아도 비슷한 color로 보이게 만들 수 있다
- 반대로, 한 조명 아래에서 같아 보이던 두 물체가 다른 조명 아래에서는 달라질 수 있다
- perception은 순수한 물리 입력만으로 끝나지 않는다. 주변 맥락과 뇌의 해석도 영향을 준다

**색상의 최적화**
색은 결국 자연의 색을 똑같이 재현하는 개념이 아니라, 어떻게 똑같이 보이게할 지 구현하는 개념임.
그래서 같은 색이더라도 보여주기 원하는 파장의 색만 내보내고 나머지 파장 색의 데이터를 삭제해서 최적화시킬 수 있음.

## Color Models

- color space는 선택 가능한 색의 전체 범위
- color model은 그 안의 한 color를 지정하는 방법
- additive model은 빛을 더하는 방식. 대표가 RGB
- subtractive model은 빛을 흡수하는 방식. 대표가 CMYK
- CMYK에서 K가 따로 있는 이유는 cyan, magenta, yellow만 섞어 깊은 black을 만들기 어렵고 비효율적이기 때문
- HSV는 hue, saturation, value로 color를 다뤄서 사람이 고르기 더 직관적
	- ![](../assets/images/Pasted%20image%2020260307235135.png)
- XYZ와 Lab은 perception을 더 잘 반영하려는 모델
- HTML 같은 곳에서는 `#RRGGBB` 형식의 hex code를 자주 쓴다. 예를 들어 `#ff6600`은 대체로 orange
- 어떤 model이 좋은지는 작업 목적에 따라 다르다. user가 고르기 쉬운지, blending이 쉬운지, 압축이 유리한지, 출력 device와 잘 맞는지 등을 봐야 함

## Compression and Gamut

- Y'CbCr는 luma와 chroma를 분리하는 모델. video에서 자주 사용
	- ![](../assets/images/Pasted%20image%2020260307235354.png)
	- $Y'$는 perceived luminance에 가깝고, $Cb$와 $Cr$는 gray에서 얼마나 blue-yellow, red-cyan 쪽으로 벗어났는지 나타낸다
	- 색 채널을 각 차원에서 다운샘플링하고, 루마($Y^{'}$)와 결하여 복원해서 크기를 줄일 수 있음.
- 사람은 chroma보다 luma에 더 민감해서 color channel을 많이 줄여도 image가 꽤 자연스럽게 보일 수 있다
- 그래서 chroma subsampling이 압축에 매우 유리하다
- color artifact가 보일 때는 upsampling 방식을 부드럽게 바꾸는 것만으로도 개선 가능. 예를 들면 bilinear filtering
- gamut은 어떤 device나 model이 표현할 수 있는 color 범위
- CIE 1931은 대부분의 사람이 볼 수 있는 색 범위를 정리한 reference color space
	- ![](../assets/images/Pasted%20image%2020260307235626.png)
- chromaticity diagram은 밝기를 제외한 색 범위를 평면에 나타낸 도표. device의 primary가 만드는 삼각형 안쪽이 그 device의 gamut
	- ![](../assets/images/Pasted%20image%2020260307235639.png)
- display와 printer의 gamut은 다르다. 그래서 화면에서는 강렬하던 색이 print에서는 죽어 보일 수 있다
- sRGB는 사람이 볼 수 있는 모든 색이 아니라, 일반적인 device가 다루기 쉬운 범위를 잡아둔 standard subset
	- ![](../assets/images/Pasted%20image%2020260307235655.png)


## Conversion and Correction

- 한 color space의 값을 다른 color space로 바꾸는 일은 단순 치환이 아니다
- 이상적으로는 output spectrum을 맞추는 것이 가장 좋다
- 그게 어렵다면 최소한 perceived color라도 가깝게 맞추는 것이 목표
- 하지만 device마다 gamut이 달라 완벽한 일치는 종종 불가능
- 그래서 ICC profile 같은 color management 체계가 필요하다
- gamma correction도 중요하다. 저장된 값과 실제 밝기 출력이 선형 관계가 아닐 수 있기 때문
- 오래된 CRT 기준으로는 대략 $Y \propto V^{\gamma}$ 형태라서, 원하는 밝기 $Y$를 얻으려면 입력을 $Y^{1/\gamma}$ 형태로 미리 보정해야 했다
- 현대 LCD에서는 display 출력 쪽 선형성이 더 좋아졌지만, camera sensor나 raw workflow에서는 여전히 gamma 개념을 조심해서 다뤄야 함

## Accessibility and Perception

- 사람은 color 변화에 균일하게 민감하지 않다. 수치 차이가 같아도 체감 차이는 다를 수 있다
- MacAdam ellipse는 거의 같은 색으로 느껴지는 범위를 보여주는 도표
	- ![](../assets/images/Pasted%20image%2020260307235744.png)
- 그래서 interface를 만들 때는 아주 가까운 색 두 개만으로 상태를 구분하면 위험하다
- 사람마다 원뿔 세포 구성과 response가 다를 수 있어 color vision도 차이가 난다
- accessible design에서는 color 하나에만 의미를 맡기지 않는 것이 좋다

## 요약
- color를 이해할 때는 spectrum부터 보는 습관이 중요
- 최종 appearance는 light, surface, sensor, brain의 합성 결과
- RGB, CMYK, HSV, XYZ, Lab은 각각 목적이 다른 도구
- device가 바뀌면 같은 숫자도 같은 색이 아닐 수 있다
- human vision은 brightness에 더 민감하고, 중심 시야에서 color를 더 잘 본다
- color workflow의 핵심 질문은 네 가지 정도로 정리 가능
  - 어떤 light 아래에서 보나
  - 어떤 device로 출력하나
  - 어떤 model로 저장하고 처리하나
  - 사람이 실제로 어떻게 지각하나
