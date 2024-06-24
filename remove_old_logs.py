
Import("env")
import os

# Remove old logs before creating a new one
def before_upload(source, target, env):
    logs_folder = os.path.join(env.subst("$PROJECT_DIR"), "logs")

    # "Should" exist by default
    if os.path.exists(logs_folder):
        files = os.listdir(logs_folder)
        
        if files:
            for file in files:
                file_path = os.path.join(logs_folder, file)
                os.remove(file_path)

env.AddPreAction("upload", before_upload)
