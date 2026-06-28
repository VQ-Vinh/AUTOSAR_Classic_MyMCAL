# **I/O Hardware Abstraction - IoHwAb) trong AUTOSAR**

## **1. IoHwAb là gì, nằm ở đâu và vì sao cần thiết?**

### 1.1. Định nghĩa

## IoHwAb (I/O Hardware Abstraction) là một module phần mềm nằm trong ECU Abstraction Layer của kiến trúc AUTOSAR Classic. Nhiệm vụ của nó là cung cấp một giao diện chung để đọc, ghi và điều khiển tất cả các tín hiệu ngõ vào / ngõ ra (I/O) của ECU—kể cả digital, analog hay PWM—mà không phụ thuộc vào cách thức phần cứng cụ thể được triển khai.

### 1.2. Vị trí trong kiến trúc phần mềm

![](data:image/png;base64...)

##

## Trên MCAL: có các driver chuyên biệt của vi điều khiển (ví dụ: Dio\_ReadChannel(), Adc\_ReadGroup(), Pwm\_SetDutyCycle()).

## Dưới IoHwAb: MCAL chịu trách nhiệm thao tác thực sự với thanh ghi, còn IoHwAb chỉ “gói” lại các driver này bằng API nhất quán.

## Trên IoHwAb: ứng dụng SWC không cần biết chân nào, thanh ghi nào mà chỉ gọi IoHwAb\_ReadButton(), IoHwAb\_SetLed(), IoHwAb\_GetTemperature()…

#### Ví dụ minh hoạ “Không dùng IoHwAb”

| // Đọc nút trên PA0 rồi bật PC13 nếu nhấn void while(1) {  if ((GPIOA->IDR & (1<<0)) == 0) {// đọc trực tiếp IDR  GPIOC->BSRR = GPIO\_BSRR\_BR13;// kéo PC13 xuống (LED on)  } else {  GPIOC->BSRR = GPIO\_BSRR\_BS13;// đẩy PC13 lên (LED off)  } } |
| --- |

## Ứng dụng thao tác thẳng thanh ghi, khó bảo trì, phải sửa code nếu chuyển chân hoặc MCU khác.

#### Ví dụ minh hoạ “Có IoHwAb”

| // Định nghĩa macro cho các kênh I/O #define BUTTON1 1 #define LED1 1 #define BUTTON\_PRESSED 0 // do nút pull-up, nhấn trả về 0 #define LED\_STATE\_ON 1 #define LED\_STATE\_OFF 0  int main(void) {  IoHwAb\_Init(); // Khởi tạo IoHwAb (bao gồm cấu hình chân)   while (1) {  // Đọc trạng thái nút bằng macro  uint8\_t pressed = IoHwAb\_ReadButton(BUTTON1);   if (pressed == BUTTON\_PRESSED) {  // Truyền trạng thái ON bằng macro  IoHwAb\_SetLed(LED1, LED\_STATE\_ON);  } else {  IoHwAb\_SetLed(LED1, LED\_STATE\_OFF);  }   // Thêm delay nếu cần  delay\_ms(50);  } } |
| --- |

### 1.4. Lợi ích chính

## Tách biệt phần cứng / phần mềm

## Tái sử dụng và di động: SWC không đổi khi chuyển MCU hoặc board

## Tập trung cấu hình: IO mapping, scale ADC, xử lý error, debounce…

## Giảm phức tạp cho ứng dụng: SWC chỉ quan tâm business logic, không quản lý thanh ghi

## **2. Các API chính của IoHwAb: Hướng dẫn sử dụng chi tiết**

## Module **IoHwAb** cung cấp một tập hợp API chuẩn để khởi tạo, vận hành và truy xuất các tín hiệu I/O. Dưới đây là danh sách các hàm **public** (tiền tố IoHwAb\_), cùng **prototype**, **mô tả chức năng**, và **ví dụ minh hoạ** từng hàm.

### **2.1 Khởi tạo module**

#### **2.1.1 IoHwAb\_Init<Init\_Id>**

| void IoHwAb\_Init<Init\_Id>(  const IoHwAb<Init\_Id>\_ConfigType\* ConfigPtr ); |
| --- |

## **Mô tả**: Khởi tạo toàn bộ hoặc một phần IoHwAb instance được đánh dấu bằng <Init\_Id>. Chức năng:

## Thiết lập cấu hình I/O (mapping chân, độ phân giải ADC, duty-cycle PWM…)

## Đặt trạng thái ban đầu (ví dụ: tất cả output tắt, buffers value = 0)

## Nếu module hỗ trợ nhiều instance, <Init\_Id> cho biết khối nào đang được khởi tạo

## **Yêu cầu**: Tham số ConfigPtr hiện không dùng, **phải** truyền NULL\_PTR .

## **Reentrancy**: Non-Reentrant

## **Ví dụ** (khi chỉ có một instance, dùng Init\_Id = 0):

| // IoHwAb\_ConfigType có thể được định nghĩa trong IoHwAb\_Cfg.h #define IOHWAB\_INSTANCE\_0 0  int main(void) {  // Truyền NULL\_PTR theo chuẩn AUTOSAR  IoHwAb\_Init0(NULL\_PTR);  // ... } |
| --- |

##

### **2.2 Thông tin phiên bản**

#### **2.2.1 IoHwAb\_GetVersionInfo**

| void IoHwAb\_GetVersionInfo(  Std\_VersionInfoType\* versioninfo ); |
| --- |

## **Mô tả**: Trả về thông tin phiên bản của module (vendor ID, module ID, sw major/minor/patch).

## **Tham số**:

## versioninfo – con trỏ đến Std\_VersionInfoType để nhận dữ liệu.

## **Reentrancy**: Reentrant .

## **Ví dụ**:

| Std\_VersionInfoType vi; IoHwAb\_GetVersionInfo(&vi); // Bây giờ vi.sw\_major\_version, vi.sw\_minor\_version... có giá trị |
| --- |

### **2.3 Vòng lặp xử lý nền**

#### **2.3.1 IoHwAb\_MainFunction**

| void IoHwAb\_MainFunction(void); |
| --- |

## **Mô tả**: Được gọi định kỳ (OS task hoặc vòng while(1) trong main) để:

## Thực thi background jobs (ví dụ khởi conversion ADC, đọc multi-channel DIO, update internal buffers)

## Đảm bảo dữ liệu luôn “fresh” khi SWC gọi API đọc .

## **Tần suất gợi ý**: 1–10 ms, tuỳ yêu cầu tín hiệu.

## **Ví dụ**:

| // Tạo task hoặc trong main: while (1) {  IoHwAb\_MainFunction(); // đọc ADC, cập nhật giá trị digital...  delay\_ms(10); } |
| --- |

### **2.4 Đọc tín hiệu analog (ADC)**

#### **2.4.1 IoHwAb\_Adc\_ReadSignal**

| Adc\_ValueGroupType IoHwAb\_Adc\_ReadSignal(  Adc\_GroupType group,  Adc\_ChannelType channel,  IoHwAb\_StatusType\* status ); |
| --- |

## **Mô tả**: Trả về giá trị ADC (raw) hoặc sau khi **scale** sang đơn vị vật lý (tùy cài đặt).

## **Cơ chế**:

## **Synchronous**: gọi Adc\_StartGroupConversion() & chờ Adc\_ReadGroup()

## **Asynchronous**: dùng giá trị đã đo trong buffer (do IoHwAb\_MainFunction hoặc callback IoHwAb\_AdcNotification<group> cập nhật) .

## **Tham số**:

## group, channel – xác định kênh ADC theo cấu hình

## status – nhận trạng thái (OK, BUSY, TIMEOUT…)

## **Ví dụ**:

| IoHwAb\_StatusType st; Adc\_ValueGroupType raw = IoHwAb\_Adc\_ReadSignal(  TEMP\_SENSOR\_GROUP, TEMP\_SENSOR\_CHANNEL, &st); if (st == IOHWAB\_OK) {  float voltage = raw \* ADC\_STEP\_VOLT; // nếu raw chưa scale } |
| --- |

### **2.5 Điều khiển tín hiệu PWM**

#### **2.5.1 IoHwAb\_SetPwmDuty**

| void IoHwAb\_SetPwmDuty(  Pwm\_ChannelType channel,  Pwm\_PeriodType duty ); |
| --- |

## **Mô tả**: Thiết lập **duty cycle** (0…100%) cho kênh PWM đã cấu hình. Bên trong gọi Pwm\_SetDutyCycle() của MCAL.

## **Ví dụ**:

| // Đặt 50% duty cho kênh PWM điều khiển quạt IoHwAb\_SetPwmDuty(FAN\_PWM\_CHANNEL, 50); |
| --- |

#### **2.5.2 IoHwAb\_Pwm\_ConvertToPeriod (tuỳ dự án)**

| Pwm\_PeriodType IoHwAb\_Pwm\_ConvertToPeriod(  Pwm\_NamedChannelsType channel,  IoHwAb\_FrequencyType freq ); |
| --- |

## **Mô tả**: Chuyển **tần số** mong muốn thành **period** tương ứng với hardware, rồi có thể dùng với Pwm\_SetPeriodAndDuty() .

### **2.6 Đọc/ghi tín hiệu digital (DIO)**

#### **2.6.1 IoHwAb\_Digital\_ReadSignal**

| Std\_ReturnType IoHwAb\_Digital\_ReadSignal(  IoHwAb\_SignalType id,  boolean\* value ); |
| --- |

## **Mô tả**: Đọc mức logic (TRUE/FALSE) của tín hiệu số mang ID symbolic id. Bên trong gọi Dio\_ReadChannel() hoặc Dio\_ReadPort().

## **Ví dụ**:

| boolean btn; if (IoHwAb\_Digital\_ReadSignal(BUTTON1, &btn) == E\_OK && btn) {  // nút nhấn } |
| --- |

#### **2.6.2 IoHwAb\_Digital\_WriteSignal**

| Std\_ReturnType IoHwAb\_Digital\_WriteSignal(  IoHwAb\_SignalType id,  boolean value ); |
| --- |

## **Mô tả**: Ghi mức logic cho tín hiệu số id. Bên trong gọi Dio\_WriteChannel().

## **Ví dụ**:

| // Bật LED1 IoHwAb\_Digital\_WriteSignal(LED1, LED\_STATE\_ON); |
| --- |

### **2.7 Callback thông báo sự kiện hardware**

## Các hàm callback này phải được **MCAL driver** cấu hình gọi khi xảy ra interrupt/event.

| **Hàm** | **Khi nào MCAL gọi?** |
| --- | --- |
| IoHwAb\_AdcNotification<#groupID>() | Khi ADC conversion nhóm <groupID> hoàn tất . |
| IoHwAb\_PwmNotification<#channel>() | Khi PWM input-capture hoặc cạnh edge PWM input xảy ra . |
| IoHwAb\_IcuNotification<#channel>() | Khi input-capture (ICU) phát hiện edge trên <channel> . |
| IoHwAb\_GptNotification<#channel>() | Khi GPT timer <channel> tràn hoặc hết giá trị . |
| IoHwAb\_OcuNotification<#channel>() | Khi Output-Compare <channel> match value . |
| IoHwAb\_Pwm\_NotifyReadyForPowerState<#Mode>() | Khi PWM driver đã sẵn sàng cho power-state <Mode> . |
| IoHwAb\_Adc\_NotifyReadyForPowerState<#Mode>() | Khi ADC driver đã sẵn sàng cho power-state <Mode> . |

## **Mô tả chung**: Các callback không có tham số và void return. Bên trong, IoHwAb lưu kết quả/đánh dấu flag và **kích hoạt RTE event** cho SWC nếu cần. **2.8 Giao diện chẩn đoán với DCM**

#### **2.8.1 IoHwAb\_Dcm\_<EcuSignalName>**

| void IoHwAb\_Dcm\_<EcuSignalName>(  uint8 action,  <EcuSignalDataType> signalValue ); |
| --- |

##

## **Mô tả**: Cho phép **DCM** điều khiển hoặc khoá giá trị của tín hiệu ECU qua UDS IOControl:

## action = IOHWAB\_RETURNCONTROLTOECU / IOHWAB\_RESETTODEFAULT / IOHWAB\_FREEZECURRENTSTATE / IOHWAB\_SHORTTERMADJUSTMENT

## signalValue = giá trị mới (chỉ dùng khi action = SHORTTERMADJUSTMENT) .

#### **2.8.2 IoHwAb\_Dcm\_Read<EcuSignalName>**

| Std\_ReturnType IoHwAb\_Dcm\_Read<EcuSignalName>(  <EcuSignalDataType>\* value ); |
| --- |

## **Mô tả**: DCM đọc lại giá trị hiện tại của tín hiệu (ví dụ để xác nhận hay log).

## **Quy tắc đặt tên & comment**

## **Tên hàm**: IoHwAb\_<Chức\_năng>\_<Signal|Group|Channel|Mode> hoặc template <Init\_Id>, <groupID>, <channel>, <Mode>…

## **File header**: Tên file cần là IoHwAb\_<ComponentName>\_<reference>.h/.c .

## **Comment**: Dùng Doxygen-style:

| /\*\*  \* @brief Notification called when ADC group conversion completed.  \* @details Called by the ADC driver interrupt; updates internal buffer and triggers RTE event.  \* @note Non-reentrant, executing in interrupt context.  \*/ void IoHwAb\_AdcNotification0(void); |
| --- |

## **Macro ID**: Dùng IOHWAB\_<SignalName>\_ID, IOHWAB\_<Init\_Id>\_CONFIG, v.v. để ánh xạ tham số.

##

## **3. Cách IoHwAb phối hợp với các module MCAL: DIO, ADC, PWM, Port**

## Theo **bảng 5.1.2** trong tài liệu AUTOSAR, IoHwAb “gọi API của tất cả các driver MCAL” (ADC, DIO, PWM, Port, …) để thực thi thao tác I/O thực tế trên phần cứng, và chỉ “nhận notification” từ một số driver có khả năng interrupt (ví dụ ADC, PWM) . Phần này sẽ đi sâu vào từng loại ngoại vi, với **ví dụ hiện thực** trong IoHwAb.c và **cách sử dụng** trong main.c.

### **3.1 Digital I/O (DIO)**

#### **3.1.1 Cách phối hợp**

## **Đọc input**

## IoHwAb gọi Dio\_ReadChannel(channelId) hoặc Dio\_ReadPort(portId) để lấy mức HIGH/LOW.

## Có thể thực hiện **debounce** hoặc **voting** trước khi trả kết quả.

## **Ghi output**

## IoHwAb gọi Dio\_WriteChannel(channelId, level) để xuất mức.

## Có thể bổ sung **read-back**: sau khi ghi, gọi lại Dio\_ReadChannel để xác nhận, và báo lỗi qua DEM nếu không khớp.

#### **3.1.2 Ví dụ triển khai**

| // IoHwAb\_Digital.h typedef enum {  IOHWAB\_SIG\_BUTTON1,  IOHWAB\_SIG\_LED1,  IOHWAB\_SIG\_COUNT } IoHwAb\_SignalType;  // IoHwAb\_Digital.c Std\_ReturnType IoHwAb\_Digital\_ReadSignal(  IoHwAb\_SignalType id, boolean\* value) {  switch(id) {  case IOHWAB\_SIG\_BUTTON1:  // Nút pull-up: nhấn = LOW  \*value = Dio\_ReadChannel(DIO\_CHANNEL\_BUTTON1);  // Debounce đơn giản 5 lần liền  for (int i = 0; i < 5; i++) {  if (Dio\_ReadChannel(DIO\_CHANNEL\_BUTTON1) != \*value) {  \*value = FALSE; // không ổn định => giả sử không nhấn  break;  }  }  return E\_OK;  default:  return E\_NOT\_OK;  } }  Std\_ReturnType IoHwAb\_Digital\_WriteSignal(  IoHwAb\_SignalType id, boolean value) {  Std\_ReturnType err = E\_OK;  switch(id) {  case IOHWAB\_SIG\_LED1:  Dio\_WriteChannel(DIO\_CHANNEL\_LED1, value);  // Read-back kiểm tra  if (Dio\_ReadChannel(DIO\_CHANNEL\_LED1) != value) {  err = E\_NOT\_OK;  }  return err;  default:  return E\_NOT\_OK;  } } |
| --- |

#### **3.1.3 Cách gọi trong main.c**

| int main(void) {  IoHwAb\_Init0(NULL\_PTR);  while (1) {  boolean pressed;  if (IoHwAb\_Digital\_ReadSignal(IOHWAB\_SIG\_BUTTON1, &pressed) == E\_OK  && pressed == STD\_LOW) {  // Nhấn nút → bật LED  IoHwAb\_Digital\_WriteSignal(IOHWAB\_SIG\_LED1, STD\_HIGH);  } else {  IoHwAb\_Digital\_WriteSignal(IOHWAB\_SIG\_LED1, STD\_LOW);  }  delay\_ms(50);  } } |
| --- |

### **3.2 Analog Input (ADC)**

#### **3.2.1 Cách phối hợp**

## **Chủ động đo liên tục (Background)**

## Trong IoHwAb\_MainFunction(), gọi định kỳ:

| Adc\_StartGroupConversion(TEMP\_GROUP); if (Adc\_GetGroupStatus(TEMP\_GROUP) == ADC\_STREAM\_COMPLETED) {  Adc\_ReadGroup(TEMP\_GROUP, &adcBuffer);  // Scale sang °C: ví dụ 10mV/°C, Vref=3.3V, 12-bit ADC  tempValue = (adcBuffer \* 3300U / 4095U) / 10U; } |
| --- |

## **On-demand**

## Khi SWC hoặc IoHwAb\_GetTemperature() gọi, kích hoạt conversion, bật notification

## Callback IoHwAb\_AdcNotification0() (do MCAL gọi) sẽ lưu kết quả và đặt cờ sẵn sàng

#### **3.2.2 Ví dụ triển khai**

| // Trong IoHwAb.c static Adc\_ValueGroupType adcBuffer; static uint16\_t tempValue; static boolean tempReady = FALSE;  /\* Called every 10 ms from main or OS task \*/ void IoHwAb\_MainFunction(void) {  Adc\_StartGroupConversion(TEMP\_ADC\_GROUP);  if (Adc\_GetGroupStatus(TEMP\_ADC\_GROUP) == ADC\_STREAM\_COMPLETED) {  Adc\_ReadGroup(TEMP\_ADC\_GROUP, &adcBuffer);  // Scale: mỗi bit ~0.8 mV, sensor 10 mV/°C  tempValue = (adcBuffer \* 800U) / 10U;  tempReady = TRUE;  } }  /\* API trả về nhiệt độ (°C) \*/ uint16\_t IoHwAb\_GetTemperature(IoHwAb\_StatusType\* status) {  if (tempReady) {  \*status = IOHWAB\_STATUS\_OK;  tempReady = FALSE; // Clear flag  return tempValue;  }  \*status = IOHWAB\_STATUS\_BUSY;  return 0; }  /\* Callback MCU gọi khi ADC hoàn thành (on-demand) \*/ void IoHwAb\_AdcNotification0(void) {  Adc\_ReadGroup(TEMP\_ADC\_GROUP, &adcBuffer);  tempValue = (adcBuffer \* 800U) / 10U;  tempReady = TRUE; } |
| --- |

#### **3.2.3 Sử dụng trong main.c**

| int main(void) {  IoHwAb\_Init0(NULL\_PTR);  while (1) {  IoHwAb\_MainFunction(); // cập nhật nhiệt độ  IoHwAb\_StatusType st;  uint16\_t T = IoHwAb\_GetTemperature(&st);  if (st == IOHWAB\_STATUS\_OK) {  // Xử lý giá trị T...  }  delay\_ms(50);  } } |
| --- |

### **3.3 PWM Output**

#### **3.3.1 Cách phối hợp**

## Gọi Pwm\_SetDutyCycle(channel, duty) để cập nhật **duty cycle** (0…100%).

## Nếu cần thay đổi tần số, gọi Pwm\_SetPeriodAndDuty(channel, period, duty) hoặc tính period = clkTimer / freq trong IoHwAb.

## Có thể giả lập PWM bằng DIO+GPT nếu phần cứng thiếu kênh .

#### **3.3.2 Ví dụ triển khai**

| // IoHwAb\_PWM.c void IoHwAb\_SetPwmDuty(Pwm\_ChannelType channel, uint16\_t duty) {  // Giả sử duty tính theo phần trăm  Pwm\_SetDutyCycle(channel, duty); }  uint16\_t IoHwAb\_Pwm\_ConvertToPeriod(  Pwm\_ChannelType channel, uint32\_t freqHz) {  // Giả sử MCU chạy 72 MHz, prescaler=72 → 1 MHz timer  const uint32\_t timerClk = 1000000U;  return (uint16\_t)(timerClk / freqHz); }  // Callback khi PWM driver sẵn sàng cho power-state void IoHwAb\_Pwm\_NotifyReadyForPowerState0(void) {  // Tắt PWM hoặc đưa vào sleep-safe output } |
| --- |

#### **3.3.3 Cách gọi trong main.c**

| int main(void) {  IoHwAb\_Init0(NULL\_PTR);  // Muốn quạt chạy 1 kHz, 50% duty  uint16\_t period = IoHwAb\_Pwm\_ConvertToPeriod(FAN\_PWM\_CH, 1000);  Pwm\_SetPeriodAndDuty(FAN\_PWM\_CH, period, 50);  // Hoặc trực tiếp:  IoHwAb\_SetPwmDuty(FAN\_PWM\_CH, 50);  while (1) {  // Thay đổi duty tuỳ nhiệt độ hay yêu cầu  delay\_ms(100);  } } |
| --- |

##

### **3.4 Port Configuration**

#### **3.4.1 Cách phối hợp**

## **Port driver** cấu hình **hướng** (IN/OUT), **chức năng** (GPIO, ADC, PWM…) và **pull-up/down** cho mỗi pin khi khởi tạo.

## IoHwAb **tin cậy** cấu hình này và **không tái cấu hình** trừ khi có yêu cầu đặc biệt (ví dụ đổi chân từ ADC sang DIO).

#### **3.4.2 Ví dụ khởi tạo chân trong IoHwAb\_Init**

## Dưới đây là một ví dụ **hoàn chỉnh** về cách bạn cấu hình và khởi tạo tất cả các chân I/O (GPIO, ADC, PWM) **trong thân hàm** IoHwAb\_Init, sử dụng cấu hình tổng hợp truyền vào qua tham số cfg:

| #include "IoHwAb.h" #include "Port.h" #include "Adc.h" #include "Pwm.h"  void IoHwAb\_Init(const IoHwAb\_ConfigType\* cfg) {  /\* 1) Kiểm tra tham số \*/  if (cfg == NULL\_PTR) {  return; /\* theo SWS\_IoHwAb\_00158, cfg phải là NULL\_PTR nếu không dùng \*/  }   /\* 2) Khởi tạo Port:   \* - Cấu hình tất cả chân I/O theo Port\_ConfigType  \* - Port\_ConfigType chứa:  \* + PinConfigs: con trỏ tới mảng Port\_PinConfigType[]  \* + PinCount: số lượng phần tử trong mảng  \*/  Port\_Init(cfg->portConfig);   /\* 3) Khởi tạo ADC driver để đọc tín hiệu analog \*/  Adc\_Init(cfg->adcConfig);   /\* 4) Khởi tạo PWM driver để xuất tín hiệu PWM \*/  Pwm\_Init(cfg->pwmConfig);} |
| --- |

##### **Chi tiết về cấu trúc Port\_ConfigType**

| typedef struct {  const Port\_PinConfigType\* PinConfigs; /\* Mảng cấu hình từng pin \*/  uint32 PinCount; /\* Số pin cần cấu hình \*/ } Port\_ConfigType; |
| --- |

##### **Ví dụ định nghĩa trong file cấu hình (IoHwAb\_Cfg.c)**

| static const Port\_PinConfigType HwAb\_Pins[] = {  /\* PA0: DIO\_OUT, thay đổi được, default HIGH, no pull \*/  { PORT\_ID\_A, 0, PORT\_PIN\_MODE\_DIO, PORT\_PIN\_OUT, TRUE, PORT\_PIN\_LEVEL\_HIGH, PORT\_PIN\_PULL\_NONE, TRUE },  /\* PA1: DIO\_IN, cố định, pull-up \*/  { PORT\_ID\_A, 1, PORT\_PIN\_MODE\_DIO, PORT\_PIN\_IN, FALSE,PORT\_PIN\_LEVEL\_LOW, PORT\_PIN\_PULL\_UP, FALSE },  /\* PB0: DIO\_OUT, cố định, default LOW, no pull \*/  { PORT\_ID\_B, 0, PORT\_PIN\_MODE\_DIO, PORT\_PIN\_OUT, FALSE,PORT\_PIN\_LEVEL\_LOW, PORT\_PIN\_PULL\_NONE, FALSE },  /\* PC13: DIO\_IN, thay đổi mode runtime, pull-down \*/  { PORT\_ID\_C,13, PORT\_PIN\_MODE\_DIO, PORT\_PIN\_IN, TRUE, PORT\_PIN\_LEVEL\_LOW, PORT\_PIN\_PULL\_DOWN, TRUE } };  const Port\_ConfigType HwAb\_PortCfg = {  .PinConfigs = HwAb\_Pins,  .PinCount = sizeof(HwAb\_Pins) / sizeof(HwAb\_Pins[0]) }; |
| --- |

## và cấu hình tổng hợp cho IoHwAb:

| const IoHwAb\_ConfigType IoHwAb\_Config = {  .portConfig = &HwAb\_PortCfg,  .adcConfig = &IoHwAb0\_AdcConfig,  .pwmConfig = &IoHwAb0\_PwmConfig }; |
| --- |

## Cuối cùng, trong main.c hoặc giai đoạn startup, bạn gọi:

| IoHwAb\_Init0(&IoHwAb\_Config); |
| --- |

##

## **4. Ví dụ minh họa: Kết hợp IoHwAb với MCAL trong ứng dụng (gọi IoHwAb trong main.c)**

**Bài toán**: Đọc giá trị nhiệt độ từ cảm biến analog (ADC) và điều khiển bộ gia nhiệt (heater) qua tín hiệu PWM để giữ nhiệt độ trong khoảng mong muốn.

### **4.1 Sơ đồ khối tổng quan**

| flowchart LR  A[Cảm biến nhiệt độ] -->|Analog| B[ADC (MCAL)]  B -->|Adc\_ReadGroup| C[IoHwAb]  C -->|IoHwAb\_GetTemperature()| D[SWC Điều khiển nhiệt độ]  D -->|tính toán duty| C  C -->|Pwm\_SetDutyCycle| E[PWM (MCAL)]  E -->|PWM wave| F[Bộ gia nhiệt] |
| --- |

* **SWC Điều khiển nhiệt độ** (hoặc main.c trong demo nhỏ) gọi IoHwAb để
  + **Lấy** nhiệt độ (°C) qua IoHwAb\_GetTemperature()
  + **Đặt** công suất heater (% duty) qua IoHwAb\_SetHeaterPower(duty)
* **IoHwAb** ẩn giấu chi tiết:
  + Chuyển đổi ADC raw → °C (scale)
  + Gọi MCAL ADC / PWM driver
  + Quản lý buffer, flag sẵn sàng

### **4.2 Các hàm chính trong IoHwAb.c**

| // IoHwAb.h #ifndef IOHWAB\_H #define IOHWAB\_H  #include "Std\_Types.h" #include "IoHwAb\_Cfg.h"  /\* Khởi tạo IoHwAb \*/ void IoHwAb\_Init(const IoHwAb\_ConfigType\* cfg);  /\* Hàm nền, phải gọi định kỳ \*/ void IoHwAb\_MainFunction(void);  /\* API lấy nhiệt độ (°C) \*/ Std\_ReturnType IoHwAb\_GetTemperature(uint16\* temperature);  /\* API đặt công suất heater (%) \*/ Std\_ReturnType IoHwAb\_SetHeaterPower(uint8 percent);  #endif |
| --- |

| // IoHwAb.c #include "IoHwAb.h" #include "Port.h" #include "Adc.h" #include "Pwm.h"  /\* Internal buffer và flag \*/ static uint16 gTemperatureRaw; static uint16 gTemperatureC; static boolean gTempReady = FALSE;  /\*\*  \* @brief Initialize IoHwAb: cấu hình chân, init MCAL  \*/ void IoHwAb\_Init(const IoHwAb\_ConfigType\* cfg) {  if (cfg == NULL\_PTR) {  return;  }   /\* 1) Cấu hình chân I/O \*/  Port\_Init(cfg->portConfig);   /\* 2) Khởi tạo ADC & PWM driver \*/  Adc\_Init(cfg->adcConfig);  Pwm\_Init(cfg->pwmConfig);   /\* Bật notification cho ADC nhóm cảm biến \*/  Adc\_EnableNotification(TEMP\_ADC\_GROUP); }  /\*\*  \* @brief Phương thức nền, gọi định kỳ (task hay vòng while)  \*/ void IoHwAb\_MainFunction(void) {  /\* Nếu chưa có conversion đang chạy, khởi chuyển đổi \*/  if (Adc\_GetGroupStatus(TEMP\_ADC\_GROUP) == ADC\_IDLE) {  Adc\_StartGroupConversion(TEMP\_ADC\_GROUP);  }  /\* Khi conversion hoàn tất sẽ có callback cập nhật buffer \*/ }  /\*\*  \* @brief Callback MCAL ADC gọi khi conversion xong  \*/ void IoHwAb\_AdcNotification(void) {  Adc\_ReadGroup(TEMP\_ADC\_GROUP, &gTemperatureRaw);  /\* Scale raw → °C (VD: 12-bit, Vref=3.3V, 10mV/°C) \*/  gTemperatureC = (uint16)((gTemperatureRaw \* 3300U / 4095U) / 10U);  gTempReady = TRUE; }  /\*\*  \* @brief SWC gọi để lấy nhiệt độ  \*/ Std\_ReturnType IoHwAb\_GetTemperature(uint16\* temperature) {  if (gTempReady == TRUE) {  \*temperature = gTemperatureC;  gTempReady = FALSE;  return E\_OK;  }  return E\_NOT\_OK; /\* Dữ liệu chưa sẵn sàng \*/ }  /\*\*  \* @brief SWC gọi để đặt công suất heater (0-100%)  \*/ Std\_ReturnType IoHwAb\_SetHeaterPower(uint8 percent) {  if (percent > 100U) {  return E\_NOT\_OK;  }  /\* Quy đổi % → duty ticks của PWM \*/  uint16 maxDuty = Pwm\_GetPeriod(HEATER\_PWM\_CHANNEL);  uint16 duty = (uint16)((percent \* maxDuty) / 100U);  Pwm\_SetDutyCycle(HEATER\_PWM\_CHANNEL, duty);  return E\_OK; } |
| --- |

### **4.3 Cấu hình (IoHwAb\_Cfg.c/h)**

| // IoHwAb\_Cfg.h #ifndef IOHWAB\_CFG\_H #define IOHWAB\_CFG\_H  #include "Port.h" #include "Adc.h" #include "Pwm.h"  /\* Định nghĩa ADC group và PWM channel \*/ #define TEMP\_ADC\_GROUP 0 #define HEATER\_PWM\_CHANNEL 1  /\* Port pin cấu hình \*/ extern const Port\_PinConfigType HwAb\_Pins[]; extern const Port\_ConfigType HwAb\_PortCfg; extern const Adc\_ConfigType HwAb\_AdcCfg; extern const Pwm\_ConfigType HwAb\_PwmCfg;  /\* Cấu trúc tổng quát \*/ typedef struct {  const Port\_ConfigType\* portConfig;  const Adc\_ConfigType\* adcConfig;  const Pwm\_ConfigType\* pwmConfig; } IoHwAb\_ConfigType;  /\* Instance config \*/ extern const IoHwAb\_ConfigType IoHwAb\_Config;  #endif // IoHwAb\_Cfg.c #include "IoHwAb\_Cfg.h"  /\* Port pins \*/ const Port\_PinConfigType HwAb\_Pins[] = {  /\* PA1: ADC input \*/  {PORT\_ID\_A, 1, PORT\_PIN\_MODE\_ADC, PORT\_PIN\_IN, FALSE, 0, PORT\_PIN\_PULL\_NONE, FALSE},  /\* PA8: PWM output (TIM1\_CH1) \*/  {PORT\_ID\_A, 8, PORT\_PIN\_MODE\_AF\_PUSH\_PULL, PORT\_PIN\_OUT, FALSE, PORT\_PIN\_LEVEL\_LOW, PORT\_PIN\_PULL\_NONE, FALSE}, };  /\* Port config \*/ const Port\_ConfigType HwAb\_PortCfg = {  .PinConfigs = HwAb\_Pins,  .PinCount = sizeof(HwAb\_Pins)/sizeof(HwAb\_Pins[0]) };  /\* ADC config (tool-generated) \*/ const Adc\_ConfigType HwAb\_AdcCfg = {  /\* .Groups[0] => TEMP\_ADC\_GROUP, ... \*/ };  /\* PWM config (tool-generated) \*/ const Pwm\_ConfigType HwAb\_PwmCfg = {  /\* .Channels[1] => HEATER\_PWM\_CHANNEL, period, polarity... \*/ };  /\* Tổng hợp IoHwAb config \*/ const IoHwAb\_ConfigType IoHwAb\_Config = {  .portConfig = &HwAb\_PortCfg,  .adcConfig = &HwAb\_AdcCfg,  .pwmConfig = &HwAb\_PwmCfg }; |
| --- |

### **4.4 Gọi IoHwAb trong main.c**

| #include "Std\_Types.h" #include "IoHwAb.h" #include "IoHwAb\_Cfg.h"  int main(void) {  /\* 1) Khởi tạo IoHwAb (Port, ADC, PWM) \*/  IoHwAb\_Init(&IoHwAb\_Config);   /\* 2) Vòng lặp nền hoặc OS task (10 ms) \*/  while (1) {  IoHwAb\_MainFunction();   /\* 3) Định kỳ đọc và xử lý (100 ms) \*/  static uint32 tick = 0;  if (++tick >= 10) {  tick = 0;  uint16 tempC;  if (IoHwAb\_GetTemperature(&tempC) == E\_OK) {  /\* So sánh với mục tiêu 80°C \*/  if (tempC < 80U) {  IoHwAb\_SetHeaterPower(50U); // 50% duty  } else {  IoHwAb\_SetHeaterPower(0U); // tắt heater  }  }  }   delay\_ms(1);  }  return 0; } |
| --- |

### **4.5 Giải thích step-by-step**

1. **Khởi tạo hệ thống**
   * IoHwAb\_Init(&IoHwAb\_Config)
     + Cấu hình chân PA1 (ADC) và PA8 (PWM) qua Port\_Init
     + Gọi Adc\_Init/Pwm\_Init để driver MCAL sẵn sàng
     + Bật notification cho ADC group
2. **Vòng lặp nền**
   * IoHwAb\_MainFunction()
     + Nếu ADC idle, gọi Adc\_StartGroupConversion
     + Khi xong, MCAL gọi callback IoHwAb\_AdcNotification để cập nhật giá trị
3. **Đọc giá trị nhiệt độ**
   * IoHwAb\_GetTemperature(&tempC)
     + Nếu flag gTempReady = TRUE, trả về tempC rồi clear flag
4. **Điều khiển heater**
   * IoHwAb\_SetHeaterPower(percent)
     + Tính duty = percent / 100 \* period
     + Gọi Pwm\_SetDutyCycle trên channel
5. **Lặp lại**
   * Cứ mỗi 1 ms lặp nền, mỗi 100 ms đọc & điều khiển một lần