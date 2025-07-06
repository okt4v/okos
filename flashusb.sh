sudo dd if=$(fzf) of=/dev/sda bs=4M status=progress
sync
sudo eject /dev/sda

echo "proccess done check if it was succesfull!"
