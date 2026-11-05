import os
import sys
import json
import time
import datetime
import urllib.parse
from http.server import SimpleHTTPRequestHandler, ThreadingHTTPServer

BASE_DIR = os.path.dirname(os.path.abspath(__file__))
DATA_DIR = os.path.join(BASE_DIR, "data")
RECEIPTS_DIR = os.path.join(BASE_DIR, "receipts")
PUBLIC_DIR = os.path.join(BASE_DIR, "public")
MENU_FILE = os.path.join(DATA_DIR, "menu.txt")
HISTORY_FILE = os.path.join(DATA_DIR, "orders_history.txt")

os.makedirs(DATA_DIR, exist_ok=True)
os.makedirs(RECEIPTS_DIR, exist_ok=True)
os.makedirs(PUBLIC_DIR, exist_ok=True)

# In-memory Token & Queue Management synchronized with data files
ACTIVE_ORDERS = []
COMPLETED_ORDERS = []
TOKEN_COUNTER = 101

# Calculate starting token from existing receipts and history
if os.path.exists(HISTORY_FILE):
    try:
        with open(HISTORY_FILE, "r", encoding="utf-8") as f:
            lines = [l.strip() for l in f if l.strip()]
            for line in lines:
                parts = line.split(";")
                if len(parts) >= 3 and parts[2].isdigit():
                    t_val = int(parts[2])
                    if t_val >= TOKEN_COUNTER:
                        TOKEN_COUNTER = t_val + 1
    except Exception as e:
        print("[!] Note on history reading:", e)

CATEGORY_NAMES = {
    1: "Breakfast",
    2: "Main Course",
    3: "Snacks",
    4: "Beverages",
    5: "Desserts",
    6: "Combos"
}

DIETARY_NAMES = {
    0: "Veg",
    1: "Non-Veg",
    2: "Vegan"
}

def load_menu():
    items = []
    if not os.path.exists(MENU_FILE):
        return items
    with open(MENU_FILE, "r", encoding="utf-8") as f:
        for line in f:
            line = line.strip()
            if not line or line.startswith("#"):
                continue
            parts = line.split(";")
            item_type = parts[0]
            try:
                if item_type == "FOOD" and len(parts) >= 10:
                    cat_id = int(parts[4])
                    diet_id = int(parts[5])
                    items.append({
                        "type": "Food",
                        "id": int(parts[1]),
                        "name": parts[2],
                        "price": float(parts[3]),
                        "categoryId": cat_id,
                        "category": CATEGORY_NAMES.get(cat_id, "General"),
                        "dietaryId": diet_id,
                        "dietary": DIETARY_NAMES.get(diet_id, "Veg"),
                        "spiceLevel": parts[6],
                        "prepTimeMinutes": int(parts[7]),
                        "stock": int(parts[8]),
                        "available": bool(int(parts[9]) and int(parts[8]) > 0)
                    })
                elif item_type == "BEV" and len(parts) >= 10:
                    cat_id = int(parts[4])
                    items.append({
                        "type": "Beverage",
                        "id": int(parts[1]),
                        "name": parts[2],
                        "price": float(parts[3]),
                        "categoryId": cat_id,
                        "category": CATEGORY_NAMES.get(cat_id, "Beverages"),
                        "isHot": bool(int(parts[5])),
                        "volumeMl": int(parts[6]),
                        "caffeine": bool(int(parts[7])),
                        "stock": int(parts[8]),
                        "available": bool(int(parts[9]) and int(parts[8]) > 0)
                    })
                elif item_type == "COMBO" and len(parts) >= 8:
                    cat_id = int(parts[4])
                    sub_items = [s.strip() for s in parts[6].split(",") if s.strip()]
                    avail = bool(int(parts[8])) if len(parts) >= 9 else True
                    items.append({
                        "type": "Combo",
                        "id": int(parts[1]),
                        "name": parts[2],
                        "price": float(parts[3]),
                        "categoryId": cat_id,
                        "category": CATEGORY_NAMES.get(cat_id, "Combos"),
                        "originalPrice": float(parts[5]),
                        "includedItems": sub_items,
                        "stock": int(parts[7]),
                        "available": bool(avail and int(parts[7]) > 0)
                    })
            except Exception as ex:
                continue
    return items

def update_menu_stock(item_deductions):
    # item_deductions: dict of {item_id: quantity}
    if not os.path.exists(MENU_FILE):
        return
    lines = []
    with open(MENU_FILE, "r", encoding="utf-8") as f:
        for line in f:
            raw = line.strip()
            if not raw or raw.startswith("#"):
                lines.append(line)
                continue
            parts = raw.split(";")
            item_type = parts[0]
            try:
                item_id = int(parts[1])
                if item_id in item_deductions:
                    qty = item_deductions[item_id]
                    if item_type in ("FOOD", "BEV"):
                        current_stock = int(parts[8])
                        new_stock = max(0, current_stock - qty)
                        parts[8] = str(new_stock)
                        parts[9] = "1" if new_stock > 0 else "0"
                    elif item_type == "COMBO":
                        current_stock = int(parts[7])
                        new_stock = max(0, current_stock - qty)
                        parts[7] = str(new_stock)
                        if len(parts) >= 9:
                            parts[8] = "1" if new_stock > 0 else "0"
                    raw = ";".join(parts)
            except Exception:
                pass
            lines.append(raw + "\n" if not raw.endswith("\n") else raw)
    
    with open(MENU_FILE, "w", encoding="utf-8") as f:
        f.writelines(lines)

class SmartServeHTTPHandler(SimpleHTTPRequestHandler):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, directory=PUBLIC_DIR, **kwargs)

    def do_GET(self):
        parsed = urllib.parse.urlparse(self.path)
        path = parsed.path

        if path == "/api/menu":
            self.send_json_response(200, {"success": True, "items": load_menu()})
        elif path == "/api/tokens":
            self.send_json_response(200, {
                "success": True,
                "active": ACTIVE_ORDERS,
                "completed": COMPLETED_ORDERS[-10:] # Last 10 completed
            })
        elif path == "/api/token_status":
            query = urllib.parse.parse_qs(parsed.query)
            token_str = query.get("token", [""])[0]
            if not token_str.isdigit():
                self.send_json_response(400, {"success": False, "error": "Invalid token number"})
                return
            token_num = int(token_str)
            found = None
            for ord_data in ACTIVE_ORDERS:
                if ord_data["tokenNumber"] == token_num:
                    found = ord_data
                    break
            if not found:
                for ord_data in COMPLETED_ORDERS:
                    if ord_data["tokenNumber"] == token_num:
                        found = ord_data
                        break
            if found:
                self.send_json_response(200, {"success": True, "order": found})
            else:
                self.send_json_response(404, {"success": False, "error": f"Token TK-{token_num} not found"})
        elif path == "/api/analytics":
            history = []
            total_rev = 0.0
            student_orders = 0
            faculty_orders = 0
            if os.path.exists(HISTORY_FILE):
                with open(HISTORY_FILE, "r", encoding="utf-8") as f:
                    for line in f:
                        line = line.strip()
                        if not line: continue
                        parts = line.split(";")
                        if len(parts) >= 8:
                            try:
                                rev = float(parts[5])
                                total_rev += rev
                                role = parts[4]
                                if role == "Student": student_orders += 1
                                elif role.startswith("Faculty"): faculty_orders += 1
                                history.append({
                                    "timestamp": parts[0],
                                    "orderId": parts[1],
                                    "token": parts[2],
                                    "customer": parts[3],
                                    "role": role,
                                    "amount": rev,
                                    "itemCount": parts[6],
                                    "payment": parts[7]
                                })
                            except Exception:
                                pass
            self.send_json_response(200, {
                "success": True,
                "totalRevenue": round(total_rev, 2),
                "totalOrders": len(history),
                "studentOrders": student_orders,
                "facultyOrders": faculty_orders,
                "activeCount": len(ACTIVE_ORDERS),
                "recentTransactions": history[-8:]
            })
        elif path.startswith("/receipts/"):
            fname = os.path.basename(path)
            fpath = os.path.join(RECEIPTS_DIR, fname)
            if os.path.exists(fpath):
                try:
                    with open(fpath, "r", encoding="utf-8") as rf:
                        content = rf.read()
                    self.send_response(200)
                    self.send_header("Content-Type", "text/plain; charset=utf-8")
                    self.send_header("Content-Length", str(len(content.encode("utf-8"))))
                    self.end_headers()
                    self.wfile.write(content.encode("utf-8"))
                    return
                except Exception:
                    pass
            self.send_error(404, "Receipt not found")
        else:
            # Fall back to static files (index.html, styles, scripts) in public/
            super().do_GET()

    def do_POST(self):
        global TOKEN_COUNTER
        parsed = urllib.parse.urlparse(self.path)
        path = parsed.path

        content_length = int(self.headers.get("Content-Length", 0))
        body = self.rfile.read(content_length).decode("utf-8")

        try:
            payload = json.loads(body) if body else {}
        except Exception:
            payload = {}

        if path == "/api/order":
            customer = payload.get("customer", {})
            cart_items = payload.get("items", [])
            coupon_code = payload.get("couponCode", "").strip().upper()
            payment = payload.get("payment", {})

            if not cart_items:
                self.send_json_response(400, {"success": False, "error": "Cart is empty"})
                return

            cust_name = customer.get("name", "Guest Student").strip() or "Guest Student"
            cust_role = customer.get("role", "Student")
            cust_contact = customer.get("contact", "N/A")
            cust_id = customer.get("id", "CS24-101")

            # Calculate Subtotal
            subtotal = sum(item.get("price", 0.0) * item.get("quantity", 1) for item in cart_items)
            total_items_count = sum(item.get("quantity", 1) for item in cart_items)

            # Role discount
            discount_rate = 0.10 if cust_role.startswith("Faculty") else 0.05
            user_discount = round(subtotal * discount_rate, 2)

            discounted_subtotal = subtotal - user_discount

            # Coupon Discount
            coupon_discount = 0.0
            if coupon_code == "CAMPUS50" and discounted_subtotal >= 200.0:
                coupon_discount = 50.0
            elif coupon_code == "WELCOME10":
                coupon_discount = round(discounted_subtotal * 0.10, 2)
            elif coupon_code == "SNACK5" and discounted_subtotal >= 50.0:
                coupon_discount = 5.0

            taxable = max(0.0, discounted_subtotal - coupon_discount)
            canteen_tax = round(taxable * 0.05, 2) # 5% GST
            grand_total = round(taxable + canteen_tax, 2)

            # Assign Token & IDs
            token_num = TOKEN_COUNTER
            TOKEN_COUNTER += 1
            order_id = int(time.time()) % 100000 + 1000
            now_str = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")

            prep_minutes = min(35, max(5, 5 + (total_items_count * 2)))

            pay_method = payment.get("method", "UPI / QR Payment")
            pay_ref = payment.get("ref", f"UPI{int(time.time()) % 1000000}REF")

            # Format printable receipt text
            receipt_lines = [
                "=================================================================",
                "              SMARTSERVE COLLEGE CANTEEN SYSTEM                 ",
                "                   TAX INVOICE & ORDER TOKEN                     ",
                "=================================================================",
                f" Order ID:       #{order_id}",
                f" Token Number:   >>> [ TK-{token_num} ] <<<",
                " Order Status:   Order Placed",
                f" Date & Time:    {now_str}",
                f" Customer:       {cust_name} ({cust_role})",
                f" Contact No:     {cust_contact}",
                "-----------------------------------------------------------------",
                " Qty | Description                     | Unit Price | Total Price",
                "-----------------------------------------------------------------"
            ]

            item_deductions = {}
            for item in cart_items:
                i_id = item.get("id", 0)
                qty = item.get("quantity", 1)
                item_deductions[i_id] = item_deductions.get(i_id, 0) + qty

                i_name = item.get("name", "Food Item")[:31]
                i_price = item.get("price", 0.0)
                i_total = i_price * qty
                receipt_lines.append(f" {qty:>3} | {i_name:<31} | {i_price:>10.2f} | {i_total:>10.2f}")
                notes = item.get("notes", "").strip()
                if notes:
                    receipt_lines.append(f'     Note: "{notes}"')

            receipt_lines.extend([
                "-----------------------------------------------------------------",
                f" Subtotal:                                            Rs. {subtotal:>8.2f}",
                f" Privilege Discount ({cust_role}):       -        Rs. {user_discount:>8.2f}"
            ])
            if coupon_discount > 0.0:
                receipt_lines.append(f" Promo Code ({coupon_code}):                  -       Rs. {coupon_discount:>8.2f}")
            receipt_lines.extend([
                f" Canteen GST (5%):                                      Rs. {canteen_tax:>8.2f}",
                "-----------------------------------------------------------------",
                f" FINAL AMOUNT PAYABLE:                                Rs. {grand_total:>8.2f}",
                "=================================================================",
                f" Payment Method: {pay_method}",
                f" Transaction ID: {pay_ref}",
                " Payment Status: PAID & VERIFIED",
                f" Est. Pickup Time: ~{prep_minutes} minutes from order placement",
                f" Please watch the Live Digital Token Board for 'TK-{token_num}'!",
                "=================================================================",
                "        Thank you for choosing SmartServe Canteen System!        ",
                "================================================================="
            ])

            receipt_content = "\n".join(receipt_lines) + "\n"

            # Save Receipt to receipts/
            receipt_filename = f"receipt_TK{token_num}.txt"
            receipt_path = os.path.join(RECEIPTS_DIR, receipt_filename)
            try:
                with open(receipt_path, "w", encoding="utf-8") as rf:
                    rf.write(receipt_content)
            except Exception as e:
                print("[!] Error saving receipt file:", e)

            # Append to orders_history.txt
            history_line = f"{now_str};{order_id};{token_num};{cust_name};{cust_role};{grand_total:.2f};{total_items_count};{pay_method}\n"
            try:
                with open(HISTORY_FILE, "a", encoding="utf-8") as hf:
                    hf.write(history_line)
            except Exception as e:
                print("[!] Error writing history:", e)

            # Deduct stock from data/menu.txt
            try:
                update_menu_stock(item_deductions)
            except Exception as e:
                print("[!] Error updating menu stock:", e)

            # Store in active orders
            order_record = {
                "orderId": order_id,
                "tokenNumber": token_num,
                "timestamp": now_str,
                "customer": {
                    "name": cust_name,
                    "role": cust_role,
                    "contact": cust_contact,
                    "id": cust_id
                },
                "items": cart_items,
                "subtotal": subtotal,
                "userDiscount": user_discount,
                "couponDiscount": coupon_discount,
                "couponCode": coupon_code,
                "tax": canteen_tax,
                "grandTotal": grand_total,
                "status": "Order Placed", # Placed -> Preparing -> Ready for Pickup -> Completed
                "statusCode": 1,
                "prepMinutes": prep_minutes,
                "payment": {
                    "method": pay_method,
                    "reference": pay_ref
                },
                "receiptFile": f"/receipts/{receipt_filename}",
                "receiptText": receipt_content
            }
            ACTIVE_ORDERS.insert(0, order_record)

            self.send_json_response(200, {
                "success": True,
                "order": order_record
            })

        elif path == "/api/advance_token":
            token_num = payload.get("tokenNumber")
            if not token_num:
                self.send_json_response(400, {"success": False, "error": "Missing tokenNumber"})
                return

            target = None
            for ord_data in ACTIVE_ORDERS:
                if ord_data["tokenNumber"] == token_num:
                    target = ord_data
                    break

            if not target:
                self.send_json_response(404, {"success": False, "error": "Active order not found"})
                return

            prev_code = target["statusCode"]
            if prev_code == 1: # Placed -> Preparing
                target["statusCode"] = 2
                target["status"] = "In Kitchen / Preparing"
            elif prev_code == 2: # Preparing -> Ready
                target["statusCode"] = 3
                target["status"] = "Ready for Pickup"
            elif prev_code == 3: # Ready -> Completed
                target["statusCode"] = 4
                target["status"] = "Completed"
                ACTIVE_ORDERS.remove(target)
                COMPLETED_ORDERS.insert(0, target)

            self.send_json_response(200, {
                "success": True,
                "tokenNumber": token_num,
                "newStatus": target["status"],
                "statusCode": target["statusCode"]
            })

        else:
            self.send_error(404, "Endpoint not found")

    def send_json_response(self, code, data):
        body = json.dumps(data, indent=2).encode("utf-8")
        self.send_response(code)
        self.send_header("Content-Type", "application/json; charset=utf-8")
        self.send_header("Access-Control-Allow-Origin", "*")
        self.send_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS")
        self.send_header("Access-Control-Allow-Headers", "Content-Type")
        self.send_header("Content-Length", str(len(body)))
        self.end_headers()
        self.wfile.write(body)

    def do_OPTIONS(self):
        self.send_response(200)
        self.send_header("Access-Control-Allow-Origin", "*")
        self.send_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS")
        self.send_header("Access-Control-Allow-Headers", "Content-Type")
        self.end_headers()

def run_server(port=8080):
    server_address = ("0.0.0.0", port)
    httpd = ThreadingHTTPServer(server_address, SmartServeHTTPHandler)
    print(f"\n========================================================")
    print(f"  SmartServe Localhost Server Active!                   ")
    print(f"  URL: http://localhost:{port}                         ")
    print(f"  Direct Token Board: http://localhost:{port}#board     ")
    print(f"========================================================\n")
    try:
        httpd.serve_forever()
    except KeyboardInterrupt:
        print("\nStopping SmartServe server...")
        httpd.server_close()

if __name__ == "__main__":
    port = 8080
    if len(sys.argv) > 1 and sys.argv[1].isdigit():
        port = int(sys.argv[1])
    run_server(port)
