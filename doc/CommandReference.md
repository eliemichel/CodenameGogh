### Command Reference
*List of essential commands to be run by CodeNameGogh*

- Transcode simple en #Vcodec avec réglages (ici réglages de la libx264):  
`ffmpeg -i path/to/input.mov -c:v #Vcodec -crf 22 -preset veryslow path/to/output.mp4`

- Transcode simple d’une séquence d’images en #codec à une fréquence d’image F (voir pour le xx qui détermine le nombre de digits à la fin de la séquence d’images, prévoir le cas particulier : *bla_%d.png où l’ordre est strictement croissant sans digits “bla_1.png -> bla_10.png -> bla_100.png ...)* :  
`ffmpeg -framerate F -i path/to/inputseq_%xxd.png -c:v #Vcodec path/to/output.mp4`

- Ajouter un audio à une séquence d’image :  
`ffmpeg -framerate F -i path/to/inputseq_%xxd.png -i path/to/inputaudio.wav -c:v #Vcodec -c:a #Acodec path/to/output.mp4`

- Copier flux vidéo déjà encodé et changer le son :  
`ffmpeg -i path/to/input.mp4 -i path/to/inputaudio.wav -c:v copy -c:a aac path/to/output.mp4`
