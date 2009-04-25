#include <WinAPI.au3>
#include <Constants.au3>
#include <WindowsConstants.au3>
#include <GuiConstantsEx.au3>

Opt("MustDeclareVars", 1)

Global $GS = ""
Global $AutoItDir = ""

Init()

HotKeySet("#o", "Test")
HotKeySet("#{ESC}", "SendClose")
HotKeySet("#n", "StartNotepad")
HotKeySet("#s", "StartCommand")
HotKeySet("#z", "StartTC")
HotKeySet("#w", "WndUtil")
HotKeySet("#`", "ToggleTopmost")
HotKeySet("#c", "SetTrans")
HotKeySet("#{F7}", "SendCurrTime")
HotKeySet("^!r", "_Restart")
HotKeySet("^!e", "EditCurrentScript")

Main()

Func Init()
	Local $asResult = StringRegExp(@AutoItExe, '^((.*\\).*\\).*$', 1)
	$AutoItDir = $asResult[0]
	$GS = $asResult[1]
EndFunc

Func Test()
	MsgBox(4096, $AutoItDir, $GS)
EndFunc

Func SendClose()
	Send("!{F4}")
EndFunc

Func StartNotepad()
	Run("notepad")
EndFunc
Func StartCommand()
	Run("cmd")
EndFunc
Func StartTC()
	Run($GS & "TC\TOTALCMD.EXE")
EndFunc

Func SendCurrTime()
	Opt("SendKeyDelay", 0)
	Opt("SendKeyDownDelay", 0) 
	Send(@YEAR & "-" & @MON & "-" & @MDAY & " " & @HOUR & ":" & @MIN & ":" & @SEC)
EndFunc

Func ToggleTopmost()
	Local $hwnd = WinGetHandle("[ACTIVE]")
	Local $pos = WinGetPos($hwnd)
	Local $x = $pos[0] + 10
	Local $y = $pos[1] + 10
	If _WinIsTopmost($hwnd) Then
		WinSetOnTop($hwnd, "", 0)
		ToolTip("取消窗口置顶", $x, $y)
	Else
		WinSetOnTop($hwnd, "", 1)
		ToolTip("窗口置顶", $x, $y)
	EndIf
	Sleep(1000)
	ToolTip("")
EndFunc

Func SetTrans()
	Local $winTitle = "B69305EE-CE54-4f58-9F7F-60F501AB5B79"
	If WinGetHandle($winTitle) <> "" Then
		WinSetState($winTitle, "", @SW_HIDE)
		Return
	EndIf
	
	Local $hwnd = WinGetHandle("[ACTIVE]")
	Local $hGUI = GUICreate($winTitle, 300, 50, -1, -1, BitOR($WS_POPUP, $WS_THICKFRAME), $WS_EX_TOOLWINDOW)
	Local $iSlider = GUICtrlCreateSlider(10, 12, 280, 30)
	Local $trans = _WinGetTrans($hwnd)
	If $trans = -1 or $trans = 0 Then $trans = 255
	GUICtrlSetLimit($iSlider, 255, 0)
	GUICtrlSetData ($iSlider, $trans)
	GUISetState()
	While 1
		Local $msg = GUIGetMsg()
		If $msg = $GUI_EVENT_CLOSE Then ExitLoop
		
		Local $newTrans = GUICtrlRead($iSlider)
		If $trans <> $newTrans Then
			$trans = $newTrans
			WinSetTrans($hwnd, "", $trans)
		EndIf
		
		;If $msg = $iSlider Then ExitLoop
		If WinGetHandle("[ACTIVE]") <> $hGUI Then ExitLoop
	WEnd
	GUIDelete($hGUI)
EndFunc

Func WndUtil()
	Local $hwnd = WinGetHandle("[ACTIVE]")
	Local $hGUI = GUICreate("窗口工具 - " & WinGetTitle($hwnd), 300, 150, -1, -1, BitOR($WS_POPUP, $WS_THICKFRAME, $WS_CAPTION), $WS_EX_TOOLWINDOW)
	Local $size = WinGetClientSize($hGUI)
	Local $btnTCOP = GUICtrlCreateButton("用TC打开程序路径", 0, 0, $size[0])
	GUICtrlSetResizing(-1, $GUI_DOCKMENUBAR)
	Local $btnTodo = GUICtrlCreateButton("其它", 0, 27, $size[0])
	GUICtrlSetResizing(-1, $GUI_DOCKMENUBAR)
	GUISetState()
	While 1
		Local $msg = GUIGetMsg()
		If $msg = $GUI_EVENT_CLOSE Then ExitLoop
		If WinGetHandle("[ACTIVE]") <> $hGUI Then ExitLoop

		Switch $msg
		Case $btnTCOP
			ShellExecute($GS & "TC\TOTALCMD.EXE", "/O /T /R=""" & _GetProcessPath(WinGetProcess($hwnd)) & """")
		EndSwitch
	WEnd
	GUIDelete($hGUI)
EndFunc

Func _GetProcessPath($pid)
	Global $oWMI = ObjGet("winmgmts:\\.\root\cimv2")
	if not IsObj($oWMI) Then Return ""
	
	Local $colProcs = $oWMI.ExecQuery("select * from win32_process where ProcessId = " & $pid)
	if not IsObj($colProcs) Then Return ""
		
	For $oProc In $colProcs
		return $oProc.ExecutablePath
	Next
	Return ""
EndFunc

Func _WinIsTopmost($hwnd)
	Local $long = _WinAPI_GetWindowLong(WinGetHandle($hwnd), $GWL_EXSTYLE)
	Return BitAND($long, $WS_EX_TOPMOST) = $WS_EX_TOPMOST
EndFunc

Func _WinGetTrans($sTitle, $sText = "")
	Local $hWnd = WinGetHandle($sTitle, $sText)
	If Not $hWnd Then Return -1
	Local $val = DllStructCreate("int")
	Local $aRet = DllCall("user32.dll", "int", "GetLayeredWindowAttributes", "hwnd", $hWnd, "ulong_ptr", 0, "int_ptr", DllStructGetPtr($val), "ulong_ptr", 0)
	If @error Or Not $aRet[0] Then Return -1
	Return DllStructGetData($val, 1)
EndFunc

Func _Restart()
	If @Compiled Then
		Run(FileGetShortName(@ScriptFullPath))
	Else
		Run(FileGetShortName(@AutoItExe) & " " & FileGetShortName(@ScriptFullPath))
	EndIf
	Exit
EndFunc

Func EditCurrentScript()
	Run(FileGetShortName($AutoItDir & "SciTE\SciTE.exe") & " " & FileGetShortName(@ScriptFullPath))
EndFunc

Func Print($msg)
	MsgBox(4096, "path", $msg)
EndFunc


Func Main()
	While 1
		Sleep(1000)
	WEnd
EndFunc
