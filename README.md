# Hệ thống điều khiển thiết bị qua SMS bằng PIC16F877A và module SIM A7680C

##  Tóm tắt

Dự án cho phép điều khiển 4 thiết bị điện từ xa bằng tin nhắn SMS, sử dụng vi điều khiển **PIC16F877A** giao tiếp **UART với SIM A7680C**. Hệ thống xử lý các lệnh tin nhắn AT command, thực hiện bật/tắt thiết bị, gửi phản hồi SMS và hiển thị trạng thái lên LCD 16x2. Ngoài ra, hệ thống còn hỗ trợ điều khiển bằng nút nhấn tự tay.

---

##  Phần cứng sử dụng

| Thiết bị                    | Vai trò                      |
| --------------------------- | ---------------------------- |
| **PIC16F877A**              | Vi điều khiển trung tâm      |
| **SIM A7680C**              | Nhận/gửi SMS qua UART        |
| **LCD 16x2**                | Hiển thị trạng thái thiết bị |
| **4 nút nhấn (PIN\_B0–B3)** | Bật/tắt TB1–TB4 bằng tay     |
| **4 rơ-le (PIN\_A0–A3)**    | Điều khiển thiết bị đầu ra   |

---

##  Phần mềm & chức năng

### Ngôn ngữ: **C (CCS Compiler)**

### Chức năng:

* Nhận lệnh SMS bằng ngắt UART (lệnh `+CMT:`)
* Các lệnh SMS hỗ trợ:

  * `BAT TB1.2.3.4`, `TAT TB1.2.3.4`, `BAT HET`, `TAT HET`
* Xử lý lệnh điều khiển thiết bị và gửi SMS phản hồi
* Hiển thị trạng thái lên LCD theo thời gian thực
* Hỗ trợ điều khiển thủ công bằng nút nhấn (có chống dội)

---

## 📁 Cấu trúc thư mục

sms-control-pic16f877a/
├── hardware/                  # Thư mục phần cứng
│   ├── PCB_Ver1.PrjPcb        # Dự án Altium (mạch nguyên lý & PCB)
│   ├── PCB_3D.zip             # Mô hình 3D của PCB
│   └── Product.zip            # File đóng gói sản phẩm phần cứng
│
├── soft/                      # Thư mục phần mềm
│   └── main.c                 # Mã nguồn chương trình chính (CCS C)
│
├── README.md                  # File mô tả dự án

---

##  Hướng phát triển

* Tích hợp thêm Websever, IOT
* Lưu lịch sử trong EEPROM
* Mở rộng thiết bị điều khiển (8 hoặc 16)

---

## Giấy phép

Dự án mang tính chất học tập/cá nhân. Tự do chia sẻ và tuân thủ quy định nguồn mở.
Hệ thống điều khiển thiết bị qua SMS bằng PIC16F877A và module SIM A7680C

📄 Tóm tắt

Dự án cho phép điều khiển 4 thiết bị điện từ xa bằng tin nhắn SMS, sử dụng vi điều khiển PIC16F877A giao tiếp UART với SIM A7680C. Hệ thống xử lý các lệnh tin nhắn AT command, thực hiện bật/tắt thiết bị, gửi phản hồi SMS và hiển thị trạng thái lên LCD 16x2. Ngoài ra, hệ thống còn hỗ trợ điều khiển bằng nút nhấn tự tay.

🔧 Phần cứng sử dụng

Thiết bị

Vai trò

PIC16F877A

Vi điều khiển trung tâm

SIM A7680C

Nhận/gửi SMS qua UART

LCD 16x2

Hiển thị trạng thái thiết bị

4 nút nhấn (PIN_B0–B3)

Bật/tắt TB1–TB4 bằng tay

4 rơ-le (PIN_A0–A3)

Điều khiển thiết bị đầu ra

💻 Phần mềm & chức năng

Ngôn ngữ: C (CCS Compiler)

Chức năng:

Nhận lệnh SMS bằng ngắt UART (lệnh +CMT:)

Các lệnh SMS hỗ trợ:

BAT TB1, TAT TB3, BAT HET, TAT HET

Xử lý lệnh điều khiển thiết bị và gửi SMS phản hồi

Hiển thị trạng thái lên LCD theo thời gian thực

Hỗ trợ điều khiển thủ công bằng nút nhấn (có chống dội)

📁 Cấu trúc thư mục

sms-control-pic16f877a/
├── src/
│   ├── main.c          # Code chính xử lý SMS + thiết bị
│   └── LCD.c           # Thư viện LCD 16x2
├── README.md           # File mô tả dự án
├── .gitignore          # (tuự chọn) Bỏ qua file *.hex, *.lst...

✨ Hướng phát triển

Tích hợp thêm RTC ghi log thao tác

Lưu lịch sử trong EEPROM

Mở rộng thiết bị điều khiển (8 hoặc 16)

🚫 Giấy phép

Dự án mang tính chất học tập/cá nhân. Tự do chia sẻ và tuân thủ quy định nguồn mở.


