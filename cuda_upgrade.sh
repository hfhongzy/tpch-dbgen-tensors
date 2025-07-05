set -e

echo "[INFO] Removing CUDA-related apt sources..."
sudo rm -f /etc/apt/sources.list.d/cuda*.list
sudo rm -f /etc/apt/sources.list.d/nvidia*.list

echo "[INFO] Removing CUDA-related GPG keyrings..."
sudo rm -f /usr/share/keyrings/cuda-archive-keyring.gpg

echo "[INFO] Purging installed CUDA packages..."
sudo apt remove --purge -y "cuda*" "nvidia-cuda*" "libcudnn*" "libcufft*" "libcurand*" "libcublas*" "libcusolver*" "libcusparse*" "libnpp*" || true
sudo apt autoremove -y

echo "[INFO] Removing CUDA directories..."
sudo rm -rf /usr/local/cuda*
sudo rm -rf /usr/include/cuda*
sudo rm -rf /usr/lib/cuda*
sudo rm -rf /var/lib/dpkg/info/cuda*

echo "[INFO] Cleanup complete. You can now reinstall fresh CUDA if needed."


wget https://developer.download.nvidia.com/compute/cuda/repos/ubuntu2204/x86_64/cuda-ubuntu2204.pin
sudo mv cuda-ubuntu2204.pin /etc/apt/preferences.d/cuda-repository-pin-600

sudo apt-key adv --fetch-keys https://developer.download.nvidia.com/compute/cuda/repos/ubuntu2204/x86_64/3bf863cc.pub

sudo bash -c 'echo "deb https://developer.download.nvidia.com/compute/cuda/repos/ubuntu2204/x86_64/ /" > /etc/apt/sources.list.d/cuda-ubuntu2204.list'

sudo apt update

sudo apt install cuda-toolkit-12-6

export CUDA_HOME=/usr/local/cuda-12.6
export PATH=$CUDA_HOME/bin:$PATH
export LD_LIBRARY_PATH=$CUDA_HOME/lib64:$LD_LIBRARY_PATH

sudo ln -sfn /usr/local/cuda-12.6 /usr/local/cuda