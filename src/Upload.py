import os
from datetime import date
from dotenv import load_dotenv
from mega import Mega

load_dotenv()

email = os.getenv("MEGA_EMAIL")
password = os.getenv("MEGA_PASSWORD")

if not email or not password:
    raise ValueError("MEGA_EMAIL or MEGA_PASSWORD is missing from .env")

mega = Mega()
m = mega.login(email, password)

# Folder name like: 2026-05-16
folder_name = date.today().isoformat()

# Create the dated folder
folder = m.create_folder(folder_name)

# Get that folder's MEGA node ID
folder_id = folder[folder_name]

# Upload file into the dated folder
uploaded_file = m.upload("Test.txt", dest=folder_id)

link = m.get_upload_link(uploaded_file)
print(f"File uploaded to folder '{folder_name}'!")
print(f"Link: {link}")