**Hướng dẫn sử dụng API module PWM (MCAL) cho STM32F103 (SPL)**

**1. Pwm\_Init**

**Mục đích** Khởi tạo toàn bộ PWM (Timer + GPIO), sẵn sàng phát xung.

**Nguyên mẫu**

| void Pwm\_Init(const Pwm\_ConfigType\* ConfigPtr); |
| --- |

**Tham số**

* **ConfigPtr**: Cấu hình tổng thể cho PWM (kênh, tần số, duty mặc định, cực tính, idle state...).

**Luồng hoạt động**

* Đọc thông tin cấu hình, bật clock cho TIMx và GPIO.
* Cấu hình chân TIMx\_CHy ở chế độ **AF Push‑Pull**.
* Thiết lập **PSC/ARR** (tần số), chọn **PWM mode** (PWM1/PWM2), bật **preload ARR/CCR**.
* Ghi **duty mặc định** (CCR), bật counter; với **TIM1** cần **MOE (BDTR)** nếu dùng CHxN.

**Ví dụ thực tế** LED mờ dần chân **PA1 (TIM2\_CH2)**, 1 kHz, duty 50% khi khởi tạo:

| Pwm\_ConfigType cfg = { /\* map TIM2\_CH2→PA1, freq=1000 Hz, duty16=0x4000, polarity=HIGH \*/ }; Pwm\_Init(&cfg); |
| --- |

**Lưu ý**

* Bật **preload** để tránh glitch khi cập nhật.

**2. Pwm\_DeInit**

**Mục đích** Trả PWM về trạng thái mặc định, đưa chân về **Idle**, tắt clock nếu cần.

**Nguyên mẫu**

| void Pwm\_DeInit(void); |
| --- |

**Tham số**

* Không có.

**Luồng hoạt động**

* Disable TIMx, tắt OC output, xoá cấu hình tạm.
* Ngắt clock TIMx nếu cần tiết kiệm năng lượng.

**Ví dụ thực tế** Tắt PWM trước khi **sleep**:

| Pwm\_DeInit(); |
| --- |

**Lưu ý**

* Với SPL có thể dùng TIM\_DeInit(TIMx) nếu dự án yêu cầu.

**3. Pwm\_SetDutyCycle**

**Mục đích** Thay đổi **duty** của một kênh PWM đang chạy.

**Nguyên mẫu**

| void Pwm\_SetDutyCycle(Pwm\_ChannelType Channel, Pwm\_DutyCycleType Duty16); |
| --- |

**Tham số**

* **Channel**: Số hiệu kênh PWM.
* **Duty16**: Duty tỷ lệ **0x0000→0% … 0x8000→100%**.

**Luồng hoạt động**

* Tính **CCR = (ARR \* Duty16) >> 15** (non‑inverting, edge‑aligned).
* Ghi CCR (đã bật preload) để cập nhật ở **cuối chu kỳ** nếu cấu hình.
* **0x0000/0x8000**: ép mức tương ứng theo polarity.

**Ví dụ thực tế** Quét sáng LED 0→100% mỗi 1 ms:

| for (uint16\_t d = 0; d <= 0x8000; d += 0x0100) {  Pwm\_SetDutyCycle(PWM\_CH\_LED, d);  delay\_ms(1); } |
| --- |

**Lưu ý**

* Luôn bật preload ARR/CCR để tránh xung sai khi đổi nhanh.

**4. Pwm\_SetPeriodAndDuty**

**Mục đích** Thay đổi **period (tần số)** và **duty** đồng thời, không glitch.

**Nguyên mẫu**

| void Pwm\_SetPeriodAndDuty(Pwm\_ChannelType Channel,  Pwm\_PeriodType PeriodTicks,  Pwm\_DutyCycleType Duty16); |
| --- |

**Tham số**

* **Channel**: Kênh PWM (**VARIABLE\_PERIOD**).
* **PeriodTicks**: Số tick cho ARR.
* **Duty16**: Duty 0x0000..0x8000.

**Luồng hoạt động**

* Ghi **ARR = PeriodTicks**; tính CCR từ Duty16; cập nhật đồng bộ (**preload + UEV**).
* PeriodTicks == 0 → đầu ra về **Idle**.

**Ví dụ thực tế** Điều khiển **servo 50 Hz** (period≈20 ms), xung 1.5 ms (~7.5%):

| Pwm\_SetPeriodAndDuty(PWM\_CH\_SERVO, ticks\_20ms, duty16\_from\_us(1500)); |
| --- |

**Lưu ý**

* Chỉ hợp lệ với kênh **VARIABLE\_PERIOD**.

**5. Pwm\_SetOutputToIdle**

**Mục đích** Đặt ngay mức chân PWM về **Idle State**.

**Nguyên mẫu**

| void Pwm\_SetOutputToIdle(Pwm\_ChannelType Channel); |
| --- |

**Tham số**

* **Channel**: Kênh PWM.

**Luồng hoạt động**

* Tạm dừng OC output hoặc ép mức ra theo Idle.

**Ví dụ thực tế** Ngắt PWM motor khi phát hiện lỗi:

| Pwm\_SetOutputToIdle(PWM\_CH\_MOTOR); |
| --- |

**Lưu ý**

* Chạy lại: **fixed‑period** dùng Pwm\_SetDutyCycle, **variable‑period** dùng Pwm\_SetPeriodAndDuty.

**6. Pwm\_GetOutputState**

**Mục đích** Đọc trạng thái tức thời **HIGH/LOW** của đầu ra PWM.

**Nguyên mẫu**

| Pwm\_OutputStateType Pwm\_GetOutputState(Pwm\_ChannelType Channel); |
| --- |

**Tham số**

* **Channel**: Kênh PWM.

**Luồng hoạt động**

* Đọc mức pin hoặc suy luận từ CNT/CCR/Polarity (tùy triển khai).

**Ví dụ thực tế**

| if (Pwm\_GetOutputState(PWM\_CH\_LED) == PWM\_HIGH) {  // ... } |
| --- |

**7. Pwm\_EnableNotification**

**Mục đích** Bật **callback** theo cạnh (RISING/FALLING/BOTH) cho kênh PWM.

**Nguyên mẫu**

| void Pwm\_EnableNotification(Pwm\_ChannelType Channel,  Pwm\_EdgeNotificationType Edge); |
| --- |

**Tham số**

* **Channel**: Kênh PWM.
* **Edge**: Cạnh kích hoạt (RISING/FALLING/BOTH).

**Luồng hoạt động**

* Bật ngắt **CCx**, xoá cờ pending; ISR gọi callback.

**Ví dụ thực tế** Đếm sản phẩm theo cạnh lên của “marker” PWM:

| Pwm\_EnableNotification(PWM\_CH\_MARK, PWM\_RISING\_EDGE); |
| --- |

**8. Pwm\_DisableNotification**

**Mục đích** Tắt **callback** của kênh PWM.

**Nguyên mẫu**

| void Pwm\_DisableNotification(Pwm\_ChannelType Channel); |
| --- |

**Tham số**

* **Channel**: Kênh PWM.

**Luồng hoạt động**

* Tắt CCx interrupt; clear cờ nếu cần.

**Ví dụ thực tế** Dừng đếm khi băng chuyền tạm dừng:

| Pwm\_DisableNotification(PWM\_CH\_MARK); |
| --- |

**9. Pwm\_GetVersionInfo**

**Mục đích** Lấy thông tin **version** của driver PWM.

**Nguyên mẫu**

| void Pwm\_GetVersionInfo(Std\_VersionInfoType\* versioninfo); |
| --- |

**Tham số**

* **versioninfo**: struct nhận version (major/minor/patch).

**Luồng hoạt động**

* Ghi version hiện tại vào struct.

**Ví dụ thực tế**

| Std\_VersionInfoType vi; Pwm\_GetVersionInfo(&vi); |
| --- |

**10. Pwm\_SetPowerState**

**Mục đích** Chuyển PWM sang trạng thái năng lượng mới (**RUN/SLEEP**).

**Nguyên mẫu**

| Std\_ReturnType Pwm\_SetPowerState(Pwm\_PowerStateType PowerState,  Pwm\_PowerStateRequestResultType\* Result); |
| --- |

**Tham số**

* **PowerState**: RUN/SLEEP.
* **Result**: Kết quả yêu cầu chuyển trạng thái.

**Luồng hoạt động**

* Nếu đã **Prepare** và mọi kênh an toàn → chuyển trạng thái, cập nhật Result.

**Ví dụ thực tế**

| Pwm\_PowerStateRequestResultType r; Pwm\_SetPowerState(PWM\_SLEEP, &r); |
| --- |

**Lưu ý**

* Chỉ bật nếu dự án dùng quản lý năng lượng cho PWM.

**11. Pwm\_GetCurrentPowerState**

**Mục đích** Lấy trạng thái năng lượng hiện tại của PWM.

**Nguyên mẫu**

| Std\_ReturnType Pwm\_GetCurrentPowerState(Pwm\_PowerStateType\* PowerState); |
| --- |

**Tham số**

* **PowerState**: nơi nhận kết quả.

**Luồng hoạt động**

* Trả về trạng thái hiện tại (RUN/SLEEP).

**Ví dụ thực tế**

| Pwm\_PowerStateType cur; Pwm\_GetCurrentPowerState(&cur); |
| --- |

**12. Pwm\_GetTargetPowerState**

**Mục đích** Lấy trạng thái năng lượng **mục tiêu** (nếu đang chuyển).

**Nguyên mẫu**

| Std\_ReturnType Pwm\_GetTargetPowerState(Pwm\_PowerStateType\* PowerState); |
| --- |

**Tham số**

* **PowerState**: nơi nhận target.

**Luồng hoạt động**

* Trả về target đã yêu cầu nhưng chưa hoàn tất.

**Ví dụ thực tế**

| Pwm\_PowerStateType target; Pwm\_GetTargetPowerState(&target); |
| --- |

**13. Pwm\_PreparePowerState**

**Mục đích** Chuẩn bị chuyển trạng thái năng lượng: dừng kênh, lưu cấu hình.

**Nguyên mẫu**

| Std\_ReturnType Pwm\_PreparePowerState(Pwm\_PowerStateType PowerState); |
| --- |

**Tham số**

* **PowerState**: trạng thái đích cần chuẩn bị.

**Luồng hoạt động**

* Đưa kênh về **Idle**, tắt notification, đảm bảo an toàn trước khi Pwm\_SetPowerState.

**Ví dụ thực tế**

| Pwm\_PreparePowerState(PWM\_SLEEP); |
| --- |

**Bảng Mapping Kiểu Dữ Liệu Cơ Bản**

| **Tên AUTOSAR** | **Kiểu dữ liệu SPL/C** | **Ghi chú** |
| --- | --- | --- |
| Pwm\_ConfigType | struct cấu hình TIM/GPIO | Tổng cấu hình module |
| Pwm\_ChannelType | uint8 / enum | Kênh (logic) → TIMx\_CHy |
| Pwm\_DutyCycleType | uint16 (0x0000..0x8000) | Thang duty 0–100% |
| Pwm\_PeriodType | uint32 (ticks) | Giá trị ARR (chu kỳ) |
| Pwm\_OutputStateType | enum {PWM\_HIGH, PWM\_LOW} | Trạng thái mức ra |
| Pwm\_EdgeNotificationType | enum {RISING, FALLING, BOTH} | Cạnh callback |
| Pwm\_PowerStateType | enum {RUN, SLEEP} | Quản lý năng lượng |
| Std\_VersionInfoType | struct {major, minor, patch} | Thông tin version |

**Khuyến nghị lập trình thực tế cho PWM STM32 (SPL, chuẩn AUTOSAR)**

1. **Tách file mapping kênh & cấu hình** Định nghĩa bảng ChannelID → TIMx\_CHy + GPIO trong Pwm\_Cfg.h/.c để nhất quán giữa MCAL/ECUA.
2. **Tính tần số & duty bằng ARR/PSC/CCR** f\_pwm = f\_tim / ((PSC+1)\*(ARR+1)); CCR = (ARR \* Duty16) >> 15.
    Bật preload ARR/CCR để commit ở UEV, tránh glitch.
3. **Chọn mode & polarity phù hợp tải** PWM1 (non‑inverting) hoặc PWM2 (inverting); chọn Idle đúng mạch công suất.
4. **TIM1 cho nửa cầu/điều khiển công suất** Dùng TIM1 + BDTR để có **dead‑time**, bật **MOE** khi cần CHxN.
5. **Notification khi cần đồng bộ** Dùng CCx interrupt để đồng bộ với ADC/logic ngoài; nhớ xoá cờ trong ISR.

**Ví dụ tổng hợp**

* **LED dimmer 1 kHz (TIM2\_CH2 → PA1)**, duty đổi theo bảng gamma:

| Pwm\_Init(&cfg\_led\_1khz); for (uint16\_t d = 0; d <= 0x8000; d += 0x0200) {  Pwm\_SetDutyCycle(PWM\_CH\_LED, d);  delay\_ms(2); } |
| --- |

* **Servo 50 Hz (TIM3\_CH1 → PA6)**, xung 1–2 ms:

| Pwm\_SetPeriodAndDuty(PWM\_CH\_SERVO, ticks\_20ms, duty16\_from\_us(1500)); |
| --- |

* **Nửa cầu có dead‑time (TIM1 CH1/CH1N)**:

| // Đã cấu hình BDTR + MOE trong init; khi cần dừng an toàn Pwm\_SetOutputToIdle(PWM\_CH\_HALFBR); |
| --- |