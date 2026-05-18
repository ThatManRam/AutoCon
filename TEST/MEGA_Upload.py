import os
from dotenv import load_dotenv
from mega import Mega
import argparse

load_dotenv()

email = os.getenv("MEGA_EMAIL")
password = os.getenv("MEGA_PASSWORD")

if not email or not password:
    raise ValueError("MEGA_EMAIL or MEGA_PASSWORD is missing from .env")

parser = argparse.ArgumentParser(description="Upload a file to a MEGA folder")
parser.add_argument("Foldername", help="Folder to create/use")
parser.add_argument("File", help="File to upload")

args = parser.parse_args()

mega = Mega()
m = mega.login(email, password)

# Folder name from command-line argument
folder_name = args.Foldername

# Creates the folder if missing, or returns the existing one
folder = m.create_folder(folder_name)

# Get the MEGA folder node ID
folder_id = folder[folder_name]

# Upload file into that folder
uploaded_file = m.upload(args.File, dest=folder_id)

link = m.get_upload_link(uploaded_file)

print(f"File uploaded to folder '{folder_name}'!")
print(f"Link: {link}")