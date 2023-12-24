# -*- coding: utf-8 -*-
"""
Created on Sat Dec 22 22:48:11 2023

@author: Karan JESWANI
"""
import random
import uuid

product_ids = ["9128283H1", "9128283L2", "912828M80", "9128283J7", "9128283F5", "912810RZ3"]

def generate_trades():
    volumes = [1000000, 2000000, 3000000, 4000000, 5000000]
    volume_state = 0
    prices = [99.0, 100.0]
    price_state = 0
    side = 0
    books = ["TRSY1", "TRSY2", "TRSY3"]
    book_state = 0

    with open("trades.csv", "w") as input_file:
        input_file.write("ProductId,TradeId,Price,Book,Quantity,Side\n")
        for product_id in product_ids:
            for _ in range(10):
                trade_id = str(uuid.uuid4()).split('-')[0]
                input_file.write(f"{product_id},{trade_id},{prices[price_state]},{books[book_state]},{volumes[volume_state]},{side}\n")
                volume_state = (volume_state + 1) % len(volumes)
                price_state = (price_state + 1) % len(prices)
                book_state = (book_state + 1) % len(books)
                side = (side + 1) % 2
    print("Generated trades.csv")

def generate_inquiries():
    volumes = [1000000, 2000000, 3000000, 4000000, 5000000]
    volume_state = 0
    side = 0

    with open("inquiries.csv", "w") as input_file:
        input_file.write("ProductId,InquiryId,Side,Quantity\n")
        for product_id in product_ids:
            for _ in range(10):
                inquiry_id = str(uuid.uuid4()).split('-')[0]
                input_file.write(f"{product_id},{inquiry_id},{side},{volumes[volume_state]}\n")
                volume_state = (volume_state + 1) % len(volumes)
                side = (side + 1) % 2
    print("Generated inquiries.csv")

def generate_prices():
    num_rows = 100000  # Number of price updates
    with open("prices.csv", "w") as input_file:
        input_file.write("ProductId,Mid,Spread\n")
        for product_id in product_ids:
            for _ in range(num_rows):
                input_file.write(f"{product_id},{get_random_fractional_price(99, 101)},{get_random_spread()}\n")
    print("Generated prices.csv")

def generate_market_data():
    num_rows = 100000  # Number of market data updates
    volumes = [10000000, 20000000, 30000000, 40000000, 50000000]
    volume_state = 0
    spread = 2
    spread_increment = 2

    with open("marketdata.csv", "w") as input_file:
        input_file.write("ProductId,BidPrice1,BidVolume1,...,OfferPrice5,OfferVolume5\n")  # Header shortened for brevity
        for product_id in product_ids:
            for _ in range(num_rows):
                mid = random.randint(99 * 256, 101 * 256)  # value multiplied by 256
                data = [product_id]
                for k in range(5):
                    data.append(get_fractional_representation(mid - spread // 2 - k))
                    data.append(str(volumes[volume_state]))
                    volume_state = (volume_state + 1) % len(volumes)
                for k in range(5):
                    data.append(get_fractional_representation(mid + spread // 2 + k))
                    data.append(str(volumes[volume_state]))
                    volume_state = (volume_state + 1) % len(volumes)
                input_file.write(",".join(data) + "\n")
                spread += spread_increment
                if spread == 8 and spread_increment == 2:
                    spread_increment = -2
                if spread == 2 and spread_increment == -2:
                    spread_increment = 2
    print("Generated marketdata.csv")

def get_random_fractional_price(low, high):
    first_part = random.randint(0, 31)
    second_part = random.randint(0, 7)
    return f"{random.randint(low, high - 1)}-{first_part:02d}{'+' if second_part == 4 else str(second_part)}"

def get_random_spread():
    return f"0-00{'23+'[random.randint(0, 1)]}"

def get_fractional_representation(increment_number):
    first_part = increment_number // 256
    remainder = increment_number % 256
    second_part = remainder // 8
    third_part = remainder % 8
    return f"{first_part}-{second_part:02d}{'+' if third_part == 4 else str(third_part)}"

# Main execution
if __name__ == '__main__':
    generate_trades()
    generate_inquiries()
    generate_prices()
    generate_market_data()
