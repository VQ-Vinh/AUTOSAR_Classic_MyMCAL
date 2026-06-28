# **Hướng dẫn sử dụng API module ADC (MCAL) cho STM32F103 (SPL)**

## **1. Adc\_Init**

**Mục đích** Khởi tạo toàn bộ ADC, chuẩn bị cho việc chuyển đổi.

**Nguyên mẫu**

| void Adc\_Init(const Adc\_ConfigType\* ConfigPtr); |
| --- |

**Tham số**

* ConfigPtr: Cấu hình tổng thể cho ADC.

**Luồng hoạt động**

* Đọc thông tin cấu hình, bật clock cho ADC và GPIO.
* Cấu hình mode, trigger, scan, số kênh, data align.
* Reset và hiệu chuẩn ADC.

**Ví dụ thực tế** Đo nhiệt độ nước chân PA0:

| Adc\_ConfigType config = { .Mode = ADC\_Mode\_Independent, .ScanConvMode = DISABLE, .ContinuousConvMode = DISABLE, .ExternalTrigConv = NONE, .DataAlign = RIGHT, .NbrOfChannel = 1 }; Adc\_Init(&config); |
| --- |

**Lưu ý**

* Luôn calibration sau khi khởi tạo.

## **2. Adc\_SetupResultBuffer**

**Mục đích** Chỉ định buffer lưu kết quả ADC cho 1 group.

**Nguyên mẫu**

| void Adc\_SetupResultBuffer(Adc\_GroupType Group, Adc\_ValueGroupType\* DataBufferPtr); |
| --- |

**Tham số**

* Group: Số hiệu group.
* DataBufferPtr: Con trỏ buffer kết quả.

**Luồng hoạt động**

* Khai báo buffer.
* Gán buffer vào group, kết quả sẽ lưu vào đây khi chuyển đổi.

**Ví dụ thực tế** Đo 3 cell pin:

| uint16 battery\_cell\_voltage[3]; Adc\_SetupResultBuffer(CELL\_GROUP, battery\_cell\_voltage); |
| --- |

## **3. Adc\_DeInit**

**Mục đích** Trả ADC về trạng thái mặc định, tắt clock, clear config.

**Nguyên mẫu**

| void Adc\_DeInit(void); |
| --- |

**Tham số**

* Không có.

**Luồng hoạt động**

* Disable ADC, trả thanh ghi về default.
* Ngắt clock ADC nếu cần tiết kiệm năng lượng.

**Ví dụ thực tế** Tắt ADC trước khi sleep:

| Adc\_DeInit(); |
| --- |

**Lưu ý**

* SPL: ADC\_DeInit(ADCx).

## **4. Adc\_StartGroupConversion**

**Mục đích** Bắt đầu chuyển đổi một group (có thể là nhiều kênh).

**Nguyên mẫu**

| void Adc\_StartGroupConversion(Adc\_GroupType Group); |
| --- |

**Tham số**

* Group: Số hiệu group.

**Luồng hoạt động**

* Cấu hình rank channel.
* Trigger chuyển đổi (software/hardware).

**Ví dụ thực tế** User bấm “Scan” để đo đồng thời 3 sensor:

| Adc\_StartGroupConversion(SENSOR\_GROUP); |
| --- |

## **5. Adc\_StopGroupConversion**

**Mục đích** Dừng chuyển đổi đang chạy của group.

**Nguyên mẫu**

| void Adc\_StopGroupConversion(Adc\_GroupType Group); |
| --- |

**Tham số**

* Group: Số hiệu group.

**Luồng hoạt động**

* Disable ADC hoặc HW trigger.

**Ví dụ thực tế** Dừng đọc khi phát hiện lỗi:

| if (isOverCurrent()) {  Adc\_StopGroupConversion(MOTOR\_GROUP); } |
| --- |

## **6. Adc\_ReadGroup**

**Mục đích** Đọc kết quả chuyển đổi của group.

**Nguyên mẫu**

| Adc\_StatusType Adc\_ReadGroup(Adc\_GroupType Group, Adc\_ValueGroupType\* DataBufferPtr); |
| --- |

**Tham số**

* Group: Số hiệu group.
* DataBufferPtr: Con trỏ buffer nhận kết quả.

**Luồng hoạt động**

* Kiểm tra trạng thái, đọc từng giá trị channel, lưu vào buffer.

**Ví dụ thực tế** Sau khi đo 3 sensor:

| uint16 sensor\_result[3]; if (Adc\_ReadGroup(SENSOR\_GROUP, sensor\_result) == ADC\_STATUS\_STREAM\_COMPLETED) {  // sensor\_result[0]: nhiệt độ, ... } |
| --- |

## **7. Adc\_EnableHardwareTrigger**

**Mục đích** Bật trigger ngoài (timer/PWM) cho group.

**Nguyên mẫu**

| void Adc\_EnableHardwareTrigger(Adc\_GroupType Group); |
| --- |

**Tham số**

* Group: Số hiệu group.

**Luồng hoạt động**

* Cấu hình ADC nhận trigger từ timer/PWM.

**Ví dụ thực tế** Lấy mẫu ADC đồng bộ theo PWM:

| Adc\_EnableHardwareTrigger(MOTOR\_PID\_GROUP); |
| --- |

## **8. Adc\_DisableHardwareTrigger**

**Mục đích** Tắt HW trigger, chuyển về software trigger.

**Nguyên mẫu**

| void Adc\_DisableHardwareTrigger(Adc\_GroupType Group); |
| --- |

**Tham số**

* Group: Số hiệu group.

**Luồng hoạt động**

* Cấu hình lại ADC chỉ nhận trigger bằng phần mềm.

**Ví dụ thực tế** Dừng log tự động khi đủ mẫu:

| Adc\_DisableHardwareTrigger(DATA\_LOGGER\_GROUP); |
| --- |

## **9. Adc\_EnableGroupNotification**

**Mục đích** Bật callback khi xong chuyển đổi group.

**Nguyên mẫu**

| void Adc\_EnableGroupNotification(Adc\_GroupType Group); |
| --- |

**Tham số**

* Group: Số hiệu group.

**Luồng hoạt động**

* Enable interrupt, callback được gọi khi xong.

**Ví dụ thực tế** Ghi log tự động khi xong 1 batch mẫu:

| Adc\_EnableGroupNotification(DATA\_LOGGER\_GROUP); |
| --- |

## **10. Adc\_DisableGroupNotification**

**Mục đích** Tắt callback, chỉ polling kết quả.

**Nguyên mẫu**

| void Adc\_DisableGroupNotification(Adc\_GroupType Group); |
| --- |

**Tham số**

* Group: Số hiệu group.

**Luồng hoạt động**

* Disable interrupt EOC.

**Ví dụ thực tế** Dừng ghi log khi không cần thiết:

| Adc\_DisableGroupNotification(DATA\_LOGGER\_GROUP); |
| --- |

## **11. Adc\_GetGroupStatus**

**Mục đích** Lấy trạng thái chuyển đổi group.

**Nguyên mẫu**

| Adc\_StatusType Adc\_GetGroupStatus(Adc\_GroupType Group); |
| --- |

**Tham số**

* Group: Số hiệu group.

**Luồng hoạt động**

* Đọc cờ trạng thái chuyển đổi (EOC).

**Ví dụ thực tế** Kiểm tra xem đã có dữ liệu mới chưa:

| if (Adc\_GetGroupStatus(BATT\_GROUP) == ADC\_STATUS\_STREAM\_COMPLETED) {  // Xử lý tiếp... } |
| --- |

## **12. Adc\_GetStreamLastPointer**

**Mục đích** Trả về con trỏ đến vị trí kết quả mới nhất của buffer (khi dùng streaming/DMA).

**Nguyên mẫu**

| Adc\_ValueGroupType\* Adc\_GetStreamLastPointer(Adc\_GroupType Group); |
| --- |

**Tham số**

* Group: Số hiệu group.

**Luồng hoạt động**

* Tính toán vị trí mẫu ADC cuối cùng vừa lưu trong buffer (DMA/circular).

**Ví dụ thực tế** Xử lý batch mẫu mới nhất:

| Adc\_ValueGroupType\* last = Adc\_GetStreamLastPointer(DATA\_LOGGER\_GROUP); processBatch(last, BATCH\_SIZE); |
| --- |

## **13. Adc\_GetVersionInfo**

**Mục đích** Lấy thông tin version của driver ADC.

**Nguyên mẫu**

| void Adc\_GetVersionInfo(Std\_VersionInfoType\* versioninfo); |
| --- |

**Tham số**

* versioninfo: struct chứa version.

**Luồng hoạt động**

* Ghi version hiện tại của driver vào struct.

**Ví dụ thực tế** Kiểm tra version khi khởi động:

| Std\_VersionInfoType info; Adc\_GetVersionInfo(&info); // info.sw\_major\_version, ... |
| --- |

## **14. Adc\_SetPowerState**

**Mục đích** Chuyển ADC sang trạng thái năng lượng mới (RUN/SLEEP).

**Nguyên mẫu**

| Std\_ReturnType Adc\_SetPowerState(Adc\_PowerStateType PowerState, Adc\_PowerStateRequestResultType\* Result); |
| --- |

**Tham số**

* PowerState: trạng thái mới (RUN/SLEEP).
* Result: kết quả chuyển trạng thái.

**Luồng hoạt động**

* Chuyển trạng thái nguồn ADC nếu hợp lệ, trả về kết quả.

**Ví dụ thực tế** Tắt ADC khi hệ thống sleep:

| Adc\_PowerStateRequestResultType result; Adc\_SetPowerState(ADC\_SLEEP, &result); |
| --- |

## **15. Adc\_GetCurrentPowerState**

**Mục đích** Lấy trạng thái năng lượng hiện tại của ADC.

**Nguyên mẫu**

| Std\_ReturnType Adc\_GetCurrentPowerState(Adc\_PowerStateType\* PowerState); |
| --- |

**Tham số**

* PowerState: struct nhận trạng thái hiện tại.

**Luồng hoạt động**

* Đọc trạng thái năng lượng hiện tại (RUN/SLEEP).

**Ví dụ thực tế** Kiểm tra trạng thái ADC:

| Adc\_PowerStateType state; Adc\_GetCurrentPowerState(&state); |
| --- |

## **16. Adc\_GetTargetPowerState**

**Mục đích** Lấy trạng thái năng lượng mà ADC đang hướng tới.

**Nguyên mẫu**

| Std\_ReturnType Adc\_GetTargetPowerState(Adc\_PowerStateType\* PowerState); |
| --- |

**Tham số**

* PowerState: struct nhận trạng thái target.

**Luồng hoạt động**

* Đọc trạng thái target đã yêu cầu nhưng chưa chuyển xong.

**Ví dụ thực tế** Kiểm tra khi vừa yêu cầu sleep:

| Adc\_PowerStateType target\_state; Adc\_GetTargetPowerState(&target\_state); |
| --- |

## **17. Adc\_PreparePowerState**

**Mục đích** Chuẩn bị chuyển trạng thái năng lượng (hoàn tất chuyển đổi, lưu trạng thái).

**Nguyên mẫu**

| Std\_ReturnType Adc\_PreparePowerState(Adc\_PowerStateType PowerState); |
| --- |

**Tham số**

* PowerState: trạng thái cần chuẩn bị.

**Luồng hoạt động**

* Hoàn thành các tác vụ còn lại.
* Sẵn sàng chuyển sang trạng thái mới.

**Ví dụ thực tế** Dừng ADC trước khi hệ thống sleep:

| Adc\_PreparePowerState(ADC\_SLEEP); |
| --- |

### **Bảng Mapping Kiểu Dữ Liệu Cơ Bản**

| **Tên AUTOSAR** | **Kiểu dữ liệu SPL/C** | **Ghi chú** |
| --- | --- | --- |
| Adc\_ConfigType | struct ADC\_InitTypeDef | Tổng cấu hình |
| Adc\_ChannelType | uint8 / enum | Số hiệu channel |
| Adc\_GroupType | uint8 / enum | Nhóm channel |
| Adc\_ValueGroupType | uint16 buffer[] | Buffer kết quả |
| Adc\_PowerStateType | enum | RUN/SLEEP |
| Adc\_PowerStateRequestResultType | enum | OK/NOT\_OK |
| Adc\_StatusType | enum/uint8 | IDLE/BUSY/COMPLETE |
| Adc\_TriggerSourceType | enum | SW/HW trigger |
| Std\_VersionInfoType | struct | major/minor/patch |

## **Khuyến nghị lập trình thực tế cho ADC STM32 (SPL, chuẩn AUTOSAR)**

### **1. Tách riêng file mapping Adc\_ChannelId, Adc\_GroupId, buffer**

* **Nên định nghĩa rõ ràng bảng mapping các channel (Adc\_ChannelType) và group (Adc\_GroupType) trong file riêng (Adc\_Cfg.h).**Dễ tra cứu, bảo trì.
  + Chuẩn hóa với mọi project.
  + Giúp phân biệt rõ đâu là channel vật lý, đâu là ID logic theo yêu cầu phần mềm/AUTOSAR.

| // Adc\_Cfg.h #define ADC\_CHANNEL\_TEMP 0u // PA0 #define ADC\_CHANNEL\_PRESS 1u // PA1 #define ADC\_CHANNEL\_HUMID 2u // PA2  #define ADC\_GROUP\_ENV 0u // Nhóm đo môi trường (temp, press, humid) |
| --- |

### **2. Luôn gọi Adc\_Init và setup buffer trước khi chuyển đổi**

* **Không tự ý thao tác thanh ghi ADC trước khi gọi hàm init (Adc\_Init) và setup buffer (Adc\_SetupResultBuffer).**
* Nếu có nhiều group, mỗi group nên có buffer riêng.

### **3. Đặt cấu hình thời gian lấy mẫu phù hợp với từng sensor**

* **Mỗi cảm biến nên có sampling time tối ưu – trở kháng càng cao, thời gian càng lớn (71/239 cycles).**
  + Đo nhiệt độ/NTC: sample time lớn.
  + Đo dòng/opamp: sample time nhỏ.
* Tránh sai số lớn hoặc mất tín hiệu khi cấu hình không phù hợp.

### **4. Sử dụng scan mode, DMA khi cần đọc nhiều kênh hoặc tốc độ cao**

* **Nếu chỉ cần lấy một giá trị – dùng chế độ single. Nếu cần lấy liên tục, tốc độ cao, nhiều kênh – luôn dùng scan mode và DMA.**
  + Tiết kiệm CPU, đồng bộ dữ liệu, ít bug.

### **5. Luôn thực hiện calibration khi khởi động lại**

* Bỏ qua calibration sẽ gây sai số ngẫu nhiên (lệch offset ADC).
* Bắt buộc calibration lại sau mỗi lần reset.

### **6. Sử dụng notification (interrupt) hoặc polling tùy ứng dụng**

* Nếu ứng dụng cần xử lý real-time, hãy enable interrupt (notification), không nên polling trong main loop.
* Ứng dụng không yêu cầu tốc độ cao, đơn giản hóa thì polling cho dễ debug.

### **7. Khi dùng nhiều group, cần quản lý trạng thái group (IDLE/BUSY/COMPLETE)**

* Nên định nghĩa enum trạng thái cho từng group.
* Tránh đọc nhầm buffer cũ hoặc ghi đè lên buffer chưa đọc.

### **Ví dụ tổng hợp thực tế**

**Đo đồng thời nhiệt độ, áp suất, độ ẩm môi trường, buffer 3 giá trị, scan mode + polling kết quả. Khi có giá trị mới, xuất ra UART.**

| // File mapping #define ADC\_CHANNEL\_TEMP 0u // PA0 #define ADC\_CHANNEL\_PRESS 1u // PA1 #define ADC\_CHANNEL\_HUMID 2u // PA2 #define ADC\_GROUP\_ENV 0u // group gồm 3 channel  uint16 env\_buffer[3]; void app\_adc\_init(void) {  // Cấu hình ADC scan 3 kênh, single mode, sampling time phù hợp  Adc\_Init(&adc\_config\_env);  Adc\_SetupResultBuffer(ADC\_GROUP\_ENV, env\_buffer); }  void app\_adc\_read(void) {  Adc\_StartGroupConversion(ADC\_GROUP\_ENV);  while (Adc\_GetGroupStatus(ADC\_GROUP\_ENV) != ADC\_STATUS\_STREAM\_COMPLETED);   // Lúc này env\_buffer[0]: nhiệt độ, [1]: áp suất, [2]: độ ẩm  uart\_printf("Temp: %d, Press: %d, Humid: %d\r\n", env\_buffer[0], env\_buffer[1], env\_buffer[2]); } |
| --- |