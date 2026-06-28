**Hướng dẫn sử dụng API module PORT (MCAL) cho STM32F103 (SPL)**

**1. Port\_Init**

**Mục đích** Khởi tạo toàn bộ cấu hình Port/Pin, sẵn sàng cho DIO và các driver ngoại vi.

**Nguyên mẫu**

| void Port\_Init(const Port\_ConfigType\* ConfigPtr); |
| --- |

**Tham số**

* **ConfigPtr**: Cấu hình tổng thể cho PORT (danh sách pin, hướng, mức khởi tạo, mode, quyền đổi runtime…).

**Luồng hoạt động**

* Bật clock cho các Port liên quan (GPIOA/B/C…).
* Ghi **mức mặc định (output latch)** trước khi chuyển pin sang **OUTPUT** để tránh glitch.
* Cấu hình **hướng** (IN/OUT), **mức khởi tạo**, **mode** (GPIO/AF/Analog…), **pull‑up/down** nếu cần.
* Khoá/ghi lại các cờ **changeable** cho từng pin (cho phép đổi Direction/Mode ở runtime hay không).

**Ví dụ thực tế** Thiết lập **PA0** làm **OUTPUT** mức **HIGH** ngay khi khởi tạo:

| Port\_ConfigType cfg = { /\* map: PA0 → OUTPUT, level=HIGH, mode=DIO \*/ }; Port\_Init(&cfg); |
| --- |

**Lưu ý**

* Port\_Init phải chạy **trước** khi dùng DIO, ADC, PWM,.. chỉ khởi tạo **pin đã khai báo**.

**2. Port\_SetPinDirection**

**Mục đích** Đổi **hướng** của 1 pin (IN/OUT) trong runtime (nếu pin cho phép đổi).

**Nguyên mẫu**

| void Port\_SetPinDirection(Port\_PinType Pin, Port\_PinDirectionType Direction); |
| --- |

**Tham số**

* **Pin**: Mã pin logic (ví dụ PORT\_PIN\_PA0).
* **Direction**: PORT\_PIN\_IN hoặc PORT\_PIN\_OUT.

**Luồng hoạt động**

* Kiểm tra pin hợp lệ và **được phép đổi hướng**.
* Ghi hướng mới; **không làm thay đổi output latch** hiện tại (nếu MCU có latch).

**Ví dụ thực tế** Chuyển **PB1** từ input sang output để điều khiển LED:

| Port\_SetPinDirection(PORT\_PIN\_PB1, PORT\_PIN\_OUT); |
| --- |

**Lưu ý**

* API chỉ khả dụng nếu bật trong cấu hình (ví dụ PortSetPinDirectionApi = TRUE).

**3. Port\_RefreshPortDirection**

**Mục đích** Khôi phục **hướng** của **tất cả** các pin về đúng **cấu hình tĩnh**, trừ các pin được đánh dấu changeable.

**Nguyên mẫu**

| void Port\_RefreshPortDirection(void); |
| --- |

**Tham số**

* Không có.

**Luồng hoạt động**

* Duyệt toàn bộ pin; áp lại **Direction** đúng như cấu hình ban đầu cho các pin **không changeable**.
* Bỏ qua pin được phép đổi hướng runtime.

**Ví dụ thực tế** Sau khi một task thay đổi hướng tạm thời, gọi để đưa hệ thống về baseline:

| Port\_RefreshPortDirection(); |
| --- |

**Lưu ý**

* Hữu ích trong chế độ an toàn/chống lỗi cấu hình tạm thời.

**4. Port\_SetPinMode**

**Mục đích** Đổi **mode** của 1 pin (DIO/ADC/SPI/PWM/USART/… ) trong runtime (nếu pin cho phép đổi mode).

**Nguyên mẫu**

| void Port\_SetPinMode(Port\_PinType Pin, Port\_PinModeType Mode); |
| --- |

**Tham số**

* **Pin**: Mã pin logic.
* **Mode**: Mode mới (tên mode phụ thuộc cấu hình MCU của bạn).

**Luồng hoạt động**

* Kiểm tra pin hợp lệ và **được phép đổi mode** (flag changeable).
* Ghi mode mới; cấu hình lại các bit liên quan (CNF/MODE hoặc cấu trúc SPL tương ứng).
* Đảm bảo thao tác **atomic** (tránh đụng độ với ISR/thread khác).

**Ví dụ thực tế** Đổi **PA2** sang **AF USART2\_TX**:

| Port\_SetPinMode(PORT\_PIN\_PA2, PORT\_PIN\_MODE\_USART2\_TX); |
| --- |

**Lưu ý**

* API chỉ khả dụng nếu bật (PortSetPinModeApi = TRUE).
* Danh mục mode/mapping cụ thể do cấu hình quyết định.

**5. Port\_GetVersionInfo**

**Mục đích** Lấy thông tin **version** của module PORT.

**Nguyên mẫu**

| void Port\_GetVersionInfo(Std\_VersionInfoType\* versioninfo); |
| --- |

**Tham số**

* **versioninfo**: con trỏ nhận version (major/minor/patch).

**Luồng hoạt động**

* Kiểm tra versioninfo != NULL (nếu bật DET).
* Ghi version hiện tại vào struct.

**Ví dụ thực tế**

| Std\_VersionInfoType vi; Port\_GetVersionInfo(&vi); |
| --- |

**Bảng Mapping Kiểu Dữ Liệu Cơ Bản**

| **Tên AUTOSAR** | **Kiểu dữ liệu SPL/C** | **Ghi chú** |
| --- | --- | --- |
| Port\_ConfigType | struct cấu hình Port/Pin | Tổng cấu hình module |
| Port\_PinType | uint8/uint16/uint32 | Mã pin logic → (Port, Pin) |
| Port\_PinDirectionType | enum { PORT\_PIN\_IN, PORT\_PIN\_OUT } | Hướng pin |
| Port\_PinModeType | uint8/16/32 (implementation‑specific) | Các mode: DIO/ADC/SPI/PWM/USART/… |
| Std\_VersionInfoType | struct { major, minor, patch } | Thông tin version |

**Khuyến nghị lập trình thực tế (STM32F103/SPL, theo tinh thần AUTOSAR)**

1. **Chuẩn hoá mapping trong Port\_Cfg.h/.c** Định nghĩa rõ: **PinID → GPIOx, Pin\_y, Mode, Direction, Level, Changeable flags**. Đồng bộ với DIO/IOHWAB.
2. **Thiết lập mức mặc định trước khi OUT** Khi init: đặt **BSRR/ODR** (mức mặc định) rồi mới chuyển sang OUTPUT để tránh glitch.
3. **Atomic access** Dùng thao tác **BSRR** (set/reset riêng biệt) hoặc bảo vệ vùng ghi để tránh xung đột.
4. **Giữ PORT thuần cấu hình** Không “đảo mức” hay xử lý logic tại PORT; các biến đổi logic để ở lớp trên (DIO/IOHWAB).
5. **Phân quyền changeable đúng mức** Chỉ bật changeable cho pin thật sự cần đổi ở runtime; còn lại cố định để an toàn.

**Ví dụ tổng hợp**

* **Khởi tạo nhóm pin**:

| // PA0: Output HIGH; PB1: Input Pull‑up; PC13: Output LOW Port\_Init(&portCfg\_default); // cấu hình đã chuẩn bị trong Port\_Cfg.c |
| --- |

* **Đổi hướng pin khi vào Low‑Power**:

| Port\_SetPinDirection(PORT\_PIN\_PA0, PORT\_PIN\_IN); // thả LED về input Port\_RefreshPortDirection(); // các pin fixed quay lại baseline |
| --- |

* **Đổi mode pin cho ngoại vi**:

| // PA9: DIO → USART1\_TX (AF) Port\_SetPinMode(PORT\_PIN\_PA9, PORT\_PIN\_MODE\_USART1\_TX); |
| --- |