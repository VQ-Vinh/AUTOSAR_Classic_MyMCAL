# **[Tài liệu] Hướng dẫn sử dụng API module DIO (MCAL) cho STM32F103 (SPL)**

## **1. Dio\_ReadChannel**

### **Mục đích**

Đọc trạng thái logic (HIGH/LOW) của một kênh DIO (GPIO pin). Được dùng khi cần kiểm tra trạng thái các input như nút nhấn, cảm biến, hoặc feedback trên output.

### **Nguyên mẫu**

| Dio\_LevelType Dio\_ReadChannel(Dio\_ChannelType ChannelId); |
| --- |

### **Tham số**

* ChannelId: Số hiệu của kênh DIO (được mapping trước, ví dụ: DIO\_CHANNEL\_PA0).

### **Trả về**

* STD\_HIGH (1): Nếu chân ở mức cao.
* STD\_LOW (0): Nếu chân ở mức thấp.

### **Luồng hoạt động**

1. **Người dùng truyền vào ChannelId.**
2. **API tra cứu thông tin mapping để xác định Port, Pin.**
3. **Đọc trạng thái vật lý chân GPIO bằng SPL (ví dụ: GPIO\_ReadInputDataBit).**
4. **Trả về STD\_HIGH/STD\_LOW.**

### **Ví dụ thực tế**

Giả sử nút nhấn đấu vào PA0:

| if (Dio\_ReadChannel(DIO\_CHANNEL\_PA0) == STD\_HIGH) {  // Nút đang được nhấn} |
| --- |

### **Lưu ý**

* Dùng cho cả chân input và output.
* Kết quả luôn là trạng thái thực tế của chân (nếu là output: đọc lại trạng thái vật lý).

## **2. Dio\_WriteChannel**

### **Mục đích**

Đặt mức logic (HIGH/LOW) cho một chân DIO (GPIO pin). Dùng điều khiển LED, relay, hoặc xuất tín hiệu điều khiển.

### **Nguyên mẫu**

| void Dio\_WriteChannel(Dio\_ChannelType ChannelId, Dio\_LevelType Level); |
| --- |

### **Tham số**

* ChannelId: Số hiệu kênh.
* Level: STD\_HIGH hoặc STD\_LOW.

### **Luồng hoạt động**

1. API xác định Port, Pin từ ChannelId.
2. Nếu chân này đã được PORT driver cấu hình là output, ghi giá trị tương ứng ra GPIO (SPL: GPIO\_SetBits/GPIO\_ResetBits).
3. Nếu là input, không có tác động vật lý.

### **Ví dụ thực tế**

Điều khiển bật/tắt LED nối PB1:

| Dio\_WriteChannel(DIO\_CHANNEL\_PB1, STD\_HIGH); // Bật LED Dio\_WriteChannel(DIO\_CHANNEL\_PB1, STD\_LOW); // Tắt LED |
| --- |

### **Lưu ý**

* Nếu ChannelId trỏ tới chân input, hàm này không tác động gì lên mức vật lý của chân.

## **3. Dio\_ReadPort**

### **Mục đích**

Đọc toàn bộ trạng thái các chân của một port (GPIOA, GPIOB...).

### **Nguyên mẫu**

| Dio\_PortLevelType Dio\_ReadPort(Dio\_PortType PortId); |
| --- |

### **Tham số**

* PortId: Số hiệu port (GPIOA=0, GPIOB=1...).

### **Trả về**

* Bitmask 16 bit trạng thái toàn bộ port.

### **Luồng hoạt động**

1. API xác định port vật lý từ PortId.
2. Đọc toàn bộ giá trị Input Data Register (SPL: GPIO\_ReadInputData).
3. Trả về giá trị đã đọc (các bit không định nghĩa sẽ là 0).

### **Ví dụ thực tế**

Đọc các nút nhấn trên toàn port A:

| uint16\_t portVal = Dio\_ReadPort(DIO\_PORT\_GPIOA); if (portVal & (1 << 3)) {  // PA3 ở mức cao } |
| --- |

### **Lưu ý**

* Có thể dùng cho cả input và output.
* Nếu port chỉ có 8 bit, các bit còn lại bằng 0.

## **4. Dial Write Port**

### **Mục đích**

Đặt đồng thời trạng thái nhiều chân của một port (toàn bộ 16 chân).

### **Nguyên mẫu**

| void Dio\_WritePort(Dio\_PortType PortId, Dio\_PortLevelType Level); |
| --- |

### **Tham số**

* PortId: Số hiệu port.
* Level: Bitmask các giá trị cần xuất.

### **Luồng hoạt động**

1. Xác định port vật lý.
2. Ghi giá trị Level vào Output Data Register (SPL: GPIO\_Write(GPIOx, Level)).
3. Chỉ những chân được PORT driver cấu hình là output mới bị tác động.

### **Ví dụ thực tế**

Xuất 0x003C lên 4 LED đấu từ PA2-PA5:

| Dio\_WritePort(DIO\_PORT\_GPIOA, 0x003C); |
| --- |

### **Lưu ý**

* Các chân input sẽ giữ nguyên.
* Dùng để điều khiển nhanh nhiều thiết bị cùng lúc (LED bar, bus dữ liệu...).

## **5. Dio\_ReadChannelGroup**

### **Mục đích**

Đọc đồng thời trạng thái một nhóm các chân liên tiếp thuộc cùng một port.

### **Nguyên mẫu**

| Dio\_PortLevelType Dio\_ReadChannelGroup(const Dio\_ChannelGroupType\* ChannelGroupIdPtr); |
| --- |

### **Tham số**

* ChannelGroupIdPtr: struct mô tả nhóm (mask, offset, port).

### **Trả về**

* Giá trị nhóm (đã dịch LSB).

### **Luồng hoạt động**

1. Đọc toàn port (GPIO\_ReadInputData).
2. Áp mask để lấy các bit thuộc nhóm.
3. Dịch phải offset về LSB.
4. Trả về giá trị nhóm.

### **Ví dụ thực tế**

Đọc bus 4 bit trên PB4–PB7:

| Dio\_ChannelGroupType dataBus = { .mask = 0x00F0, .offset = 4, .port = DIO\_PORT\_GPIOB }; uint8\_t value = Dio\_ReadChannelGroup(&dataBus); |
| --- |

### **Lưu ý**

* Dùng cho các ứng dụng bus, input keypad, group sensor, v.v.

## **6. Dio\_WriteChannelGroup**

### **Mục đích**

Ghi đồng thời giá trị ra một nhóm các chân liên tiếp trên cùng một port.

### **Nguyên mẫu**

| void Dio\_WriteChannelGroup(const Dio\_ChannelGroupType\* ChannelGroupIdPtr, Dio\_PortLevelType Level); |
| --- |

### **Tham số**

* ChannelGroupIdPtr: struct mô tả nhóm.
* Level: Giá trị nhóm (tối đa bằng số bit của mask).

### **Luồng hoạt động**

1. Đọc trạng thái toàn port.
2. Xóa các bit thuộc mask, chèn Level đã dịch offset.
3. Ghi lại giá trị mới lên port.

### **Ví dụ thực tế**

Xuất dữ liệu ra 4 LED trên PA2-PA5:

| Dio\_ChannelGroupType ledGroup = { .mask = 0x003C, .offset = 2, .port = DIO\_PORT\_GPIOA }; Dio\_WriteChannelGroup(&ledGroup, 0b1010); |
| --- |

### **Lưu ý**

* Đảm bảo mask là các bit liên tiếp.
* Input channel sẽ không bị ảnh hưởng.

## **7. Dio\_GetVersionInfo *(nếu bật qua config)***

### **Mục đích**

Trả về thông tin version của module DIO, dùng cho kiểm thử, logging, quản lý version phần mềm.

### **Nguyên mẫu**

| void Dio\_GetVersionInfo(Std\_VersionInfoType\* VersionInfo); |
| --- |

### **Tham số**

* VersionInfo: con trỏ struct để chứa version.

### **Luồng hoạt động**

* Gán các trường versionID, sw\_major, sw\_minor, sw\_patch vào struct.

### **Ví dụ thực tế**

| Std\_VersionInfoType info; Dio\_GetVersionInfo(&info); printf("DIO v%d.%d.%d\n", info.sw\_major\_version, info.sw\_minor\_version, info.sw\_patch\_version); |
| --- |

### **Lưu ý**

* Được dùng khi hệ thống cần check version các module BSW.

## **8. Dio\_FlipChannel *(nếu bật qua config)***

### **Mục đích**

Đảo trạng thái logic (toggle) của 1 chân output (0->1, 1->0).

### **Nguyên mẫu**

| Dio\_LevelType Dio\_FlipChannel(Dio\_ChannelType ChannelId); |
| --- |

### **Tham số**

* ChannelId: Số hiệu kênh.

### **Trả về**

* Trạng thái mới sau khi đảo.

### **Luồng hoạt động**

1. Đọc trạng thái hiện tại của chân.
2. Nếu là output, đảo trạng thái, ghi lại giá trị mới.
3. Trả về giá trị mới.
4. Nếu là input, không tác động vật lý, trả về trạng thái đang đọc.

### **Ví dụ thực tế**

Nhấp nháy LED mỗi khi có timer interrupt:

| void TIM2\_IRQHandler(void) {  Dio\_FlipChannel(DIO\_CHANNEL\_PC13); } |
| --- |

### **Lưu ý**

* Chỉ có tác động với chân output.

## **9. Dio\_MaskedWritePort *(nếu bật qua config)***

### **Mục đích**

Chỉ thay đổi trạng thái một số chân được chỉ định trong mask, giữ nguyên trạng thái các chân còn lại của port.

### **Nguyên mẫu**

| void Dio\_MaskedWritePort(Dio\_PortType PortId, Dio\_PortLevelType Level, Dio\_PortLevelType Mask); |
| --- |

### **Tham số**

* PortId: Số hiệu port.
* Level: Giá trị muốn xuất.
* Mask: Bit mask xác định chân nào sẽ thay đổi.

### **Luồng hoạt động**

1. Đọc trạng thái toàn port hiện tại.
2. Xóa các bit được mask, chèn giá trị Level mới (Level & Mask).
3. Ghi lại toàn port.

### **Ví dụ thực tế**

Chỉ xuất dữ liệu lên PB2, PB3 (không ảnh hưởng PB0, PB1, ...):

| Dio\_MaskedWritePort(DIO\_PORT\_GPIOB, 0x000C, 0x000C); |
| --- |

*(0x000C = 0b00001100: chỉ cập nhật PB2, PB3)*

### **Lưu ý**

* An toàn khi nhiều thiết bị chia sẻ cùng port.

## **Sơ đồ tổng quan lựa chọn API**

| **Mục đích** | **API nên dùng** |
| --- | --- |
| Đọc trạng thái 1 chân | Dio\_ReadChannel |
| Đọc trạng thái nhiều chân rời | Gọi nhiều lần Dio\_ReadChannel |
| Đọc nhiều chân liên tiếp | Dio\_ReadChannelGroup |
| Đọc cả port | Dio\_ReadPort |
| Ghi trạng thái 1 chân | Dio\_WriteChannel |
| Ghi nhiều chân liên tiếp | Dio\_WriteChannelGroup |
| Ghi cả port | Dio\_WritePort |
| Đảo trạng thái 1 chân | Dio\_FlipChannel |
| Ghi chỉ 1 số chân bất kỳ trong port | Dio\_MaskedWritePort |
| Lấy version DIO | Dio\_GetVersionInfo |

## **Khuyến nghị lập trình thực tế cho STM32 (SPL)**

* Tạo file mapping ChannelId, PortId (cấu hình cứng).
* Luôn gọi Port driver trước khi sử dụng DIO.(học ở bài 2)
* Không thay đổi direction (input/output) qua DIO – chỉ Port driver mới có quyền này.
* Chỉ sử dụng các chân đã được cấu hình đúng chức năng digital.
* Chỉ sử dụng ChannelGroup khi cần hiệu năng xử lý nhiều bit liền kề (bus, keypad...).

## **Ví dụ tổng hợp**

**Đọc trạng thái 8 nút nhấn đấu vào PB0-PB7 và bật 8 LED tương ứng nối từ PA0-PA7:**

| Dio\_ChannelGroupType buttons = { .mask = 0x00FF, .offset = 0, .port = DIO\_PORT\_GPIOB }; Dio\_ChannelGroupType leds = { .mask = 0x00FF, .offset = 0, .port = DIO\_PORT\_GPIOA };  while (1) {  uint8\_t button\_state = Dio\_ReadChannelGroup(&buttons);  Dio\_WriteChannelGroup(&leds, button\_state); // Ấn nút nào sáng LED đó } |
| --- |