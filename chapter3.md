The CodeSourcery installation recommended in the book is wrong - you'll need the arm-none-eabi version, which can be downloaded from: https://sourcery.mentor.com/GNUToolchain/release2635.

3.1 Registers are named r0, r1, r2..., as shown in Figure 2.4. The command to read a register is: ```print /x $r0```. Some (sp, lr, pc) can be accessed by their name in addition to their number (sp = r13)
3.2 I thought it was just ```watch i```, but I could not get it to work. GDB by default sets hardware watchpoints (whatever that means), disabling this with ```set can-use-hw-watchpoints 0``` and reinitialising the watchpoint with ```watch i``` does the trick.
3.3 See https://stackoverflow.com/questions/6517423/how-to-do-an-specific-action-when-certain-breakpoint-is-hit-in-gdb#6517457
```
break inc 
commands
silent
printf "i is %d\n",i
cont
end
```


