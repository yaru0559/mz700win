<!-- コメント -->
# mz700win version 0.56a
code by Marukun, Youkan 
   
Specifically, please visit there.  
Marukun: [http://retropc.net/mz-memories/mz700/](http://retropc.net/mz-memories/mz700/) <br>
Youkan: [http://www.maroon.dti.ne.jp/youkan/mz700/](http://www.maroon.dti.ne.jp/youkan/mz700/)  
- To build this project, Please use Visual Studio 2010. "Fullscreen" is disabled. Because It doesn't run yet.  
Please overwrite in a solution file of existence and a project file in sln directory, according to the version of Visual Studio you're using.
Default is for VS2010.  

---
## MZ700Win Version 0.56a
### master (2017/04/03)
- port $E5に関する微細な修正。下位3bitだけでプライオリティを判断するようにした。<hr>
Fine fix for port $E5. The priority is judged with only the lower 3 bits.

---
## MZ700Win Version 0.56
### master (2016/09/10)
- branch ver_2016_09_03 was merged.<br>
このバージョンでXInput対応 Windows用 XBOXゲームパッドに対応しました。
何故DirectInputではなくてXInputなのか？  
それは、作者がWindows用 XBOXゲームパッドを持っていたという事と、  
XInputのほうが将来性がありそうだと思ったからです。   
ゲームパッドとＭＺキーの対応は下記の通りです。  
方向キー = カーソルキー  
Ａボタン = スペースキー  
Ｂボタン = SHIFTキー  
Ｘボタン = Ｚキー  
Ｙボタン = Ｘキー  <br>
……キーコンフィグ？　今のところは無いです。将来暇が出来たらやるかもしれません。 <hr>
This version is implemented XInput for XBOX360 Gamepad for Windows.  
Why isn't it DirectInput, and XInput?  
Author (Marukun) had a 'XBOX360 Gamepad for Windows' in that.  
It's because I thought XInput seemed to be with a bright future.  
... Key Gonfig ?  
I don't have that for the moment. If leisure is done in the future, it may be done.  

---
## MZ700Win Version 0.56
### branch ver_2016_09_03 (2016/09/09)

To build, You need DirectX SDK Feb2010. It includes 'ddraw.lib' and 'XInput.lib'.  
You can get it from below link.  
[http://www.microsoft.com/downloads/details.aspx?displaylang=en&FamilyID=2c7da5fb-ffbb-4af6-8c66-651cbd28ca15](http://www.microsoft.com/downloads/details.aspx?displaylang=en&FamilyID=2c7da5fb-ffbb-4af6-8c66-651cbd28ca15)  
MZ700Win needed 'ddraw.lib' and 'XInput.lib' from this version.  
But full screen mode doesn't  yet.  
It'll be merged with master branch later.  

** DirectX SDK Jun2010 is not include 'ddraw.lib'. Please don't use this version. **  

---
### branch ver_2016_09_03 (2016/09/08)
Allocation of game pad and keyboard was implemented by fixing.  
ARROW Key = CURSOR key  
A button = SPACE key  
B button = SHIFT key  
X button = Z key  
Y button = X key  

---
## Version 0.55b (2016/08/28)
Screen size setting is expanded even x4 for a person with 4K monitor.  
