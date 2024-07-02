# 커널 모듈 프로그래밍 공부하기 - The Linux Kernel Module Programming Guide

### Linux 커널 모듈 프로그래밍 가이드(5.x) - 전뇌해커님의 번역 참고

## + .c 파일은 Makefile에서 object 파일로 만들어주고,
## + Makefile은 무조건 공백은 탭으로!
## + Makefile 내용을 다채우고 나서는 "make"명령어 작성
## + 이후 "sudo insmod [파일명].ko"로 올리고
## + 다시 "sudo rmmod [파일명].ko"로 내림.
