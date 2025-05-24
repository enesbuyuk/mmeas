import subprocess

subprocess.run(["C:\\VulkanSDK\\1.4.313.0\\Bin\\glslc.exe", "./shader.vert", "-o", "vertex.spv"], check=True)
subprocess.run(["C:\\VulkanSDK\\1.4.313.0\\Bin\\glslc.exe", "./shader.frag", "-o", "fragment.spv"], check=True)
