This post is a personal study note based on CMU 15-462 (Computer Graphics) 2020/fall lectures.
>All original lecture slides and videos are copyrighted by the instructors.

강의에 대한 정보와 자료는 아래 홈페이지에서 확인할 수 있습니다.  
https://15462.courses.cs.cmu.edu/fall2020/home

본 포스팅은 강의 내용을 바탕으로 하되, 이해를 돕기 위해 별도의 자료 조사와 개념 정리를 덧붙여 작성했습니다. 따라서 원 강의 내용에 추가적으로 개인적인 학습 자료가 다수 포함되어 있습니다.

---

## Occlusion과 샘플링 깊이
Occlusion은 여러 삼각형이 같은 화면 샘플 지점을 덮을 때, 어떤 삼각형이 보이는지 결정하는 문제.

불투명(opaque) 표면은 가장 가까운 표면만 보이며, 반투명(semi-transparent) 표면은 합성을 고려해야 한다.

### 샘플 지점 깊이 계산

![](../assets/images/Pasted%20image%2020260302223603.png)
투영된 2D 삼각형이 주어졌을 때 샘플 위치 (x, y)에서 깊이 d를 계산해야 함. 정점 깊이 di, dj, dk가 주어지면 무게중심 좌표로 선형 보간함.

깊이는 삼각형 내부에서 선형적으로 변한다는 가정하에, 색이나 텍스처 좌표와 같은 다른 속성과 동일한 보간 구조로 처리함.

---

## Depth Buffer(Z-buffer)

래스터화 파이프라인은 삼각형을 하나씩 처리하는 구조.
이 구조에서 occlusion을 해결하기 위해 depth buffer를 사용함.

Depth buffer는 각 샘플마다 지금까지 관측된 가장 가까운 프리미티브의 깊이를 저장한다.
초기값은 매우 큰 값(무한대 또는 최대 표현값)으로 설정.

### Depth test와 업데이트 규칙
각 샘플 (x, y)에 새로 계산된 깊이 d와 색 c가 들어오면 depth test를 수행함.
- 통과하면 색 버퍼와 깊이 버퍼를 갱신함.
- 실패하면 아무 것도 갱신하지 않음.

삼각형 처리 순서는 중요하지 않음. 최종적으로 각 샘플에 가장 가까운 깊이가 남기 때문.

![](../assets/images/Pasted%20image%2020260302223911.png)

### 교차 표면(interpenetration) 처리
![](../assets/images/Pasted%20image%2020260302223953.png)
Depth buffer는 삼각형 단위가 아니라 샘플 단위로 전후 관계를 결정함. 따라서 삼각형이 서로 관통해도 샘플마다 깊이를 비교하므로 정상 동작함.

---

## Depth Buffer와 Supersampling

Supersampling을 사용할 때도 depth buffer는 동일 원리로 동작함. 조건은 색 샘플마다 깊이 샘플도 하나씩 두는 것임.

- 픽셀당 N개의 supersample을 쓰면 깊이도 N개를 저장함.
- 각 supersample에서 depth test를 수행해 색을 결정함.
- 최종 픽셀 색은 supersample 색들을 평균해 리샘플링함.

결론적으로 depth buffer는 픽셀당 1개가 아니라 샘플당 1개임.

---

## Alpha와 Compositing

### Alpha 모델
- α = 1이면 완전 불투명
- α = 0이면 완전 투명
- 0 < α < 1이면 부분 투명

### 이미지의 alpha 채널
이미지에는 RGB 채널 외에 alpha 채널을 둘 수 있음. 이를 통해 전경 이미지를 배경 이미지 위에 합성할 수 있음.

### Fringing 현상
![](../assets/images/스크린샷%202026-03-02%20오후%2010.41.48.png)
Fringing은 전경 오브젝트를 배경과 합성할 때, 경계의 반투명 영역에서 색과 alpha 처리가 부정확하면 어두운 테두리가 생기는 현상.
필터링, 업샘플링, 다운샘플링 과정에서 특히 문제가 두드러짐.

---

## Over operator와 순서 의존성

### Over 연산의 성질
Over 연산은 이미지 B를 이미지 A 위에 올려 합성하는 연산.
이 연산은 일반적으로 교환법칙이 성립하지 않음.

- A over B 와 B over A 는 일반적으로 서로 다른 결과.
- 따라서 투명 합성은 렌더링 순서에 의존.

### Non-premultiplied alpha 합성
Non-premultiplied 표현에서 A와 B의 RGB는 alpha와 분리되어 저장됨. 합성은 다음과 같이 정의할 수 있음.

- A = ($A_r, A_g, A_b$)
- B = ($B_r, B_g, B_b$)

Composite color:
- $C=a_BB+(1-a_B)a_AA$

Composite alpha:
- $a_c$ = $a_B + (1-a_B)a_A$


### Premultiplied alpha 합성
Premultiplied alpha는 RGB를 미리 alpha로 곱해 저장하는 표현임.
![](../assets/images/Pasted%20image%2020260302224842.png)

합성은 다음과 같이 수행함.
- C′ = B′ + (1 − $a_B$) A′

표시용 RGB가 필요하면 un-premultiply를 수행함.
![](../assets/images/Pasted%20image%2020260302224905.png)

Premultiplied alpha는 alpha를 RGB와 동일한 방식으로 합성한다는 성질을 가짐.

---

## Premultiplied alpha와 fringing 감소

### 업샘플링에서의 fringing
Non-premultiplied로 업샘플링한 뒤 합성하면 경계에서 배경색이 섞여 halo가 생길 수 있음. Premultiplied alpha에서는 투명 영역의 RGB가 alpha와 함께 0으로 수렴하므로 필터링 과정에서 불필요한 색 누수가 줄어듦.

![](../assets/images/Pasted%20image%2020260302225102.png)

### 다운샘플링과 프리필터링
Alpha matte가 있는 텍스처를 미리 다운샘플링할 때도 같은 문제가 발생함. Premultiplied alpha는 프리필터링 결과를 배경에 합성했을 때 경계 색이 더 자연스럽게 유지되도록 함.


### 반복 합성에서의 안정성
여러 레이어를 반복해서 over로 합성할 때 premultiplied alpha는 표현이 안정적임. Non-premultiplied에서는 반복 합성 시 색이 불필요하게 어두워지는 형태가 나타날 수 있음.


---

## 반투명 프리미티브 렌더링 전략

### 기본 난점
반투명 표면은 덮어쓰기(overwrite)가 아니라 합성(over)되어야 함. Over는 순서 의존적이므로, 단일 패스에서 올바른 결과를 얻기 위해 뒤에서 앞으로(back-to-front) 정렬이 필요함. 프리미티브가 서로 교차하면 정렬 자체가 불가능할 수 있음.

### 단순 전략과 가정
Premultiplied alpha를 사용하고, 프리미티브를 뒤에서 앞으로 렌더링한다는 가정 하에 색 버퍼를 다음과 같이 갱신할 수 있음.

```text
update_color_buffer(x, y, sample_color, sample_depth):
    if pass_depth_test(sample_depth, zbuffer[x][y]):
        color[x][y] = over(sample_color, color[x][y])

over(c1, c2):
    return c1.rgba + (1 - c1.a) * c2.rgba
```

이 구현이 올바르게 동작하려면 반투명 프리미티브가 뒤에서 앞으로 그려져야 함. 또한 반투명 표면을 그릴 때 depth buffer를 어떻게 갱신할지에 대한 추가 결정이 필요함.


### 추가 버퍼 기반 접근과 한계
픽셀마다 여러 값을 저장하는 알파 버퍼 또는 리스트 기반 구조를 두는 접근이 존재함. 하지만 메모리 사용량과 관리 비용이 높아 실시간 파이프라인에서 일반적으로 부담이 큼.

### 현대적 접근
순서에 덜 의존하도록 투명도를 처리하는 order-independent transparency 계열 기법이 존재함. 대표적으로 depth peeling이 알려져 있음. 래스터화 기반에서 투명 처리의 복잡성을 완화하기 위한 목적임.

---

## 불투명과 투명 혼합 장면의 표준 처리 절차
불투명 삼각형과 반투명 삼각형이 섞인 장면에서 흔히 사용하는 절차는 2단계임.

1) 불투명 프리미티브 렌더링  
- depth buffer를 사용함. 순서는 무관함.  
- depth test를 통과하면 해당 샘플의 색을 덮어쓰고 depth도 갱신함.

2) 반투명 프리미티브 렌더링  
- depth write를 끔. 즉, depth buffer 갱신을 비활성화함.  
- depth test는 유지함. 불투명 표면 뒤에 완전히 가려진 반투명은 합성하지 않기 위함임.  
- 반투명은 뒤에서 앞으로 정렬 후, 통과 샘플에서 over 연산으로 합성함.

![](../assets/images/Pasted%20image%2020260302225449.png)

---

## End-to-end 래스터화 파이프라인 단계

### 입력 데이터
- 정점 위치 (x, y, z)
- 텍스처 좌표 (u, v)
- 텍스처 맵
- 객체-카메라 공간 변환 T (4x4)
- 원근 투영 변환 P (4x4)
- 출력 이미지 크기 (W, H)

### 처리 단계
1. 삼각형 정점을 카메라 공간으로 변환함.  
![](../assets/images/Pasted%20image%2020260302225655.png)

2. 원근 투영 변환을 적용해 정규화 좌표 공간으로 변환함.  
![](../assets/images/Pasted%20image%2020260302225717.png)

3. 클리핑 수행. 단위 큐브 밖 삼각형을 제거하거나 잘라서 내부로 만듦.  
![](../assets/images/Pasted%20image%2020260302225726.png)

4. 동차 나눗셈 후, 화면 좌표로 매핑함. 이미지 좌표계에 맞춰 스케일과 필요 시 y축 반전 등을 적용함.  
![](../assets/images/Pasted%20image%2020260302225732.png)

5. 삼각형 전처리 수행. 에지 방정식, 속성 보간 방정식 등 반복 계산을 미리 준비함.  
![](../assets/images/Pasted%20image%2020260302225739.png)

6. 샘플 커버리지 판단 및 속성 평가 수행. 샘플이 삼각형 내부면 z, u, v 등을 보간해 계산함.  
![](../assets/images/Pasted%20image%2020260302225747.png)
![](../assets/images/Pasted%20image%2020260302225752.png)

7. depth test 수행 및 필요 시 depth 갱신함.  
![](../assets/images/Pasted%20image%2020260302225831.png)

8. color buffer 갱신함. 불투명은 덮어쓰기, 투명은 over 합성이 가능함.  
![](../assets/images/Pasted%20image%2020260302225825.png)

---

## 현대 그래픽 하드웨어 관점
현대 API의 파이프라인은 크게 다음 흐름으로 요약 가능함.
- Vertex Processing
- Primitive Processing
- Rasterization을 통한 Fragment Generation
- Fragment Processing
- Screen sample operations로서 depth test와 blending 수행

강의에서 구성한 래스터화 파이프라인은 현대 GPU 파이프라인의 핵심 구조와 동일한 방향성을 가짐.
차이는 GPU가 이를 대규모 병렬 처리와 고정 기능 유닛으로 고성능 수행한다는 점.

![](../assets/images/Pasted%20image%2020260302225940.png)

---

## 레이트레이싱 관점 메모
래스터화에서는 반투명 프리미티브 처리 시 순서 의존성과 교차 문제 때문에 구현이 까다로움. 레이트레이싱은 광선과 표면의 교차를 따라 전후 관계를 구성할 수 있어 반투명 처리에서 구조적 이점이 있음.

---

## 최종 요약
- 깊이는 무게중심 좌표로 보간 가능함.
- depth buffer는 샘플별 최소 깊이를 저장하고 depth test로 occlusion을 해결함.
- supersampling에서도 샘플당 depth를 유지하면 정상 동작함.
- 투명 합성은 over 연산을 사용하며 비가환적이므로 순서가 중요함.
- fringing은 색과 alpha의 부정확한 처리 및 필터링 과정에서 발생 가능함.
- premultiplied alpha는 filtering과 반복 합성에서 안정적이며 fringing을 줄이는 데 유리함.
- 혼합 장면은 불투명 먼저(depth write on), 투명은 back-to-front(depth write off, depth test on, over 합성)로 처리하는 구성이 표준임.
