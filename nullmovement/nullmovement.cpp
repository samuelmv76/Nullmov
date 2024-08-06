#include <iostream>
#include <unordered_map>
#include <windows.h>

using namespace std;

// Mapa para almacenar el estado de las teclas físicas
unordered_map<int, bool> keyStates;
int lastKey = 0; // Última tecla activa

// Definir códigos virtuales para las teclas A y D
const int KEY_A = 0x41;
const int KEY_D = 0x44;

// Función para simular una pulsación de tecla
void keystroke(BYTE bVk, bool down) {
    if (down) {
        keybd_event(bVk, 0, 0, 0);
    }
    else {
        keybd_event(bVk, 0, KEYEVENTF_KEYUP, 0);
    }
}

// Función para manejar eventos de tecla pulsada
void onKeyDown(int vkCode) {
    if (vkCode != KEY_A && vkCode != KEY_D) {
        // Ignorar teclas que no sean A o D
        return;
    }

    if (keyStates[vkCode]) {
        // La tecla ya está presionada físicamente
        return;
    }

    keyStates[vkCode] = true;

    // Activar la nueva tecla
    keystroke(vkCode, true);
    lastKey = vkCode;
    cout << "Tecla presionada: " << hex << vkCode << endl; // Mensaje de depuración
}

// Función para manejar eventos de tecla soltada
void onKeyUp(int vkCode) {
    if (vkCode != KEY_A && vkCode != KEY_D) {
        // Ignorar teclas que no sean A o D
        return;
    }

    if (!keyStates[vkCode]) {
        // La tecla no estaba presionada físicamente
        return;
    }

    keyStates[vkCode] = false;

    // Soltar la tecla que fue soltada
    keystroke(vkCode, false);
    cout << "Tecla soltada: " << hex << vkCode << endl; // Mensaje de depuración

    // Buscar otra tecla presionada para activar
    for (const auto& keyState : keyStates) {
        if (keyState.second) {
            // Simular la pulsación nuevamente y mantenerla activa
            keystroke(keyState.first, true);
            lastKey = keyState.first;
            cout << "Tecla reactivada: " << hex << lastKey << endl; // Mensaje de depuración
            return; // Salir del bucle si se encontró una tecla activa
        }
    }

    lastKey = 0; // Resetear lastKey si no hay ninguna tecla presionada
}

HHOOK hHook = NULL;

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        KBDLLHOOKSTRUCT* pKeyBoard = (KBDLLHOOKSTRUCT*)lParam;
        int vkCode = pKeyBoard->vkCode;
        switch (wParam) {
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
            onKeyDown(vkCode);
            break;
        case WM_KEYUP:
        case WM_SYSKEYUP:
            onKeyUp(vkCode);
            break;
        }
    }
    return CallNextHookEx(hHook, nCode, wParam, lParam);
}

int main() {
    cout << "Programa iniciado. Presiona las teclas A y D para ver el resultado." << endl;

    // Instalar el gancho de teclado
    hHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, NULL, 0);
    if (hHook == NULL) {
        cout << "Error al instalar el gancho de teclado." << endl;
        return 1;
    }

    // Bucle de mensajes para mantener el programa en ejecución
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // Desinstalar el gancho de teclado
    UnhookWindowsHookEx(hHook);
    cout << "Programa terminado." << endl;

    return 0; 
}
