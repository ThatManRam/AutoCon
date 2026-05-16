import os
from dotenv import load_dotenv
from mega import Mega

load_dotenv()

email = os.getenv("MEGA_EMAIL")
password = os.getenv("MEGA_PASSWORD")

if not email or not password:
    raise ValueError("MEGA_EMAIL or MEGA_PASSWORD is missing from .env")

mega = Mega()
m = mega.login(email, password)

files = m.get_files()

deleted_count = 0

for file_id, file_data in files.items():
    node_type = file_data.get("t")

    # Skip MEGA system nodes:
    # 2 = Cloud Drive root
    # 3 = Inbox
    # 4 = Rubbish Bin
    if node_type in (2, 3, 4):
        continue

    name = file_data.get("a", {}).get("n", "Unknown")

    print(f"Moving to trash: {name} ({file_id})")
    m.delete(file_id)
    deleted_count += 1

print(f"Done. Moved {deleted_count} items to the MEGA Rubbish Bin.")