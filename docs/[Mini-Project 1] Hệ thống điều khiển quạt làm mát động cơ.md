# **Mini-Project: Hệ Thống Điều Khiển Quạt Làm Mát Theo Nhiệt Độ**

## **1. Mục tiêu**

| * Hiểu và triển khai thực tế các module AUTOSAR cơ bản (MCAL và IoHwAb). * Làm quen cách cấu hình, sử dụng các MCAL Driver: **Port, Dio, Adc, Pwm**. * Thiết kế và phát triển một module **I/O Hardware Abstraction (IoHwAb)**. * Thực hành triển khai một hệ thống nhúng đơn giản: Điều khiển tốc độ quạt DC theo giá trị cảm biến nhiệt độ. |
| --- |

## **2. Mô tả chức năng yêu cầu**

Hệ thống sử dụng vi điều khiển STM32F103C8T6 để:

* **Đọc nhiệt độ môi trường** từ một cảm biến nhiệt độ analog (ví dụ LM35 hoặc NTC).
* **Điều chỉnh tốc độ quạt DC (12V)** theo nhiệt độ đo được:
  + Dưới 30°C: quạt tắt.
  + Từ 30°C đến dưới 40°C: quạt chạy ở 50%.
  + Từ 40°C trở lên: quạt chạy ở 100%.
* **LED hiển thị trạng thái**: bật sáng khi quạt hoạt động, tắt khi quạt ngừng.
* Học viên viết mã nguồn ứng dụng (main.c) chỉ gọi trực tiếp vào API của IoHwAb, **không thao tác trực tiếp với MCAL**.

## **3. Danh sách linh kiện sử dụng**

| **Linh kiện** | **Chức năng** |
| --- | --- |
| STM32F103C8T6 "Blue Pill" | Vi điều khiển chính |
| LM35DZ hoặc NTC10K | Đo nhiệt độ (ADC) |
| MOSFET IRLZ44N hoặc IRFZ44N | Điều khiển tốc độ quạt bằng PWM |
| Quạt DC 12V (80x80 mm hoặc tương tự) | Làm mát, được điều khiển bằng PWM |
| LED 5mm + điện trở 220Ω | Báo trạng thái quạt (On/Off) |
| Breadboard, Jumper wires | Lắp ráp và đấu nối mạch |

## **4. Yêu cầu về phần mềm (AUTOSAR Classic)**

### **4.1 MCAL Drivers**

Cần triển khai (hoặc sử dụng sẵn nếu có):

* **Port Driver:**
  + Cấu hình các chân GPIO:
    - PA0: Input Analog (ADC).
    - PA8: PWM Output (Timer1 Channel 1).
    - PC13: GPIO Output (LED status).
* **ADC Driver:**
  + ADC1 single-channel mode, sử dụng DMA hoặc Interrupt để lấy giá trị liên tục từ cảm biến.
* **PWM Driver:**
  + Timer1 Channel 1: Tần số PWM 10 kHz.
  + API điều chỉnh duty cycle.
* **Dio Driver:**
  + Điều khiển chân GPIO (LED bật/tắt).

### **4.2 Module IoHwAb**

Viết riêng một module IoHwAb với các API công khai để Application sử dụng:

| /\* Hàm khởi tạo IoHwAb module \*/ Std\_ReturnType  IoHwAb\_Init(void);  /\* Đọc nhiệt độ hiện tại (trả về giá trị nhiệt độ theo °C) \*/ Std\_ReturnType  IoHwAb\_ReadTemperature(uint8 \*temp*);  /* Thiết lập tốc độ quạt (duty cycle %) \*/ Std\_ReturnType  IoHwAb\_SetFanDuty(uint8 percent);  /\* Điều khiển trạng thái LED báo \*/ Std\_ReturnType  IoHwAb\_SetLed(boolean state); |
| --- |

* Module IoHwAb chỉ được gọi xuống các MCAL Drivers (ADC, PWM, DIO, PORT).
* Không xử lý logic ứng dụng (như bật/tắt dựa vào nhiệt độ) trong IoHwAb.

## **5. Cấu trúc cây thư mục đề nghị**

| Project\_FanControl\_AUTOSAR │ ├── main.c  ├── Makefile  ├── README.md  │ ├── IoHwAb /\* ECU Abstraction Layer \*/ │ ├── IoHwAb.c /\* Triển khai hàm gọi xuống MCAL \*/ │ └── IoHwAb.h  │ ├── MCAL  │ ├── Port │ │ ├── Port.c │ │ └── Port.h │ │ │ ├── Dio │ │ ├── Dio.c │ │ └── Dio.h │ │ │ ├── Adc │ │ ├── Adc.c │ │ └── Adc.h │ │ │ └── Pwm │ ├── Pwm.c │ └── Pwm.h │ ├── Config /\* Configuration Files cho MCAL \*/  ├── Port\_Cfg.c  ├── Dio\_Cfg.c  ├── Adc\_Cfg.c  └── Pwm\_Cfg.c |
| --- |

## **6. Các bước thực hiện đề nghị cho học viên**

**Bước 1: Thiết kế sơ đồ mạch**

* Vẽ sơ đồ nguyên lý kết nối STM32 với các linh kiện (LM35/NTC, MOSFET, LED, Quạt).

**Bước 2: Cấu hình Port Driver**

* Định nghĩa các chân GPIO theo đúng yêu cầu chức năng (ADC, PWM, GPIO).

**Bước 3: Triển khai ADC Driver**

* Đọc giá trị từ ADC, đảm bảo trả về giá trị nhiệt độ hợp lý theo công thức chuyển đổi phù hợp.

**Bước 4: Triển khai PWM Driver**

* Xây dựng hàm API điều chỉnh duty cycle PWM phù hợp (0-100%).

**Bước 5: Triển khai Dio Driver**

* Điều khiển LED trạng thái đơn giản (On/Off).

**Bước 6: Triển khai IoHwAb Module**

* Viết module IoHwAb gọi xuống các MCAL Drivers, cung cấp các API thống nhất cho Application.

**Bước 7: Viết chương trình ứng dụng**

* Ở main.c, gọi IoHwAb API để đọc nhiệt độ và điều khiển quạt theo các ngưỡng yêu cầu, bật/tắt LED phù hợp trạng thái quạt.