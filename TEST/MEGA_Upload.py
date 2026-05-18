import os
from pathlib import Path
from dotenv import load_dotenv
from mega import Mega
import argparse

load_dotenv()

email = os.getenv("MEGA_EMAIL")
password = os.getenv("MEGA_PASSWORD")

if not email or not password:
    raise ValueError("MEGA_EMAIL or MEGA_PASSWORD is missing from .env")

parser = argparse.ArgumentParser(description="Upload files to MEGA")
parser.add_argument("Foldername", help="Remote MEGA folder name")
parser.add_argument("Path", help="Local file or directory to upload")

args = parser.parse_args()

folder_name = args.Foldername
local_path = Path(args.Path)

if not local_path.exists():
    raise FileNotFoundError(f"Path does not exist: {local_path}")

mega = Mega()
m = mega.login(email, password)

# Create remote dated folder in MEGA
folder = m.create_folder(folder_name)
folder_id = folder[folder_name]

# If a single file was provided, upload it
if local_path.is_file():
    uploaded_file = m.upload(str(local_path), dest=folder_id)
    print(f"Uploaded: {local_path.name}")

# If a directory was provided, upload every file directly inside it
elif local_path.is_dir():
    for file_path in local_path.iterdir():
        if file_path.is_file():
            m.upload(str(file_path), dest=folder_id)
            print(f"Uploaded: {file_path.name}")

    print(f"All files from '{local_path}' uploaded to MEGA folder '{folder_name}'.")

else:
    raise ValueError(f"Unsupported path type: {local_path}")