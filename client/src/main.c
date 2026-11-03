#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <enet/enet.h>
#include <raylib.h>

#include "protocol.h"

/* Test ENet : connexion a un serveur sur 127.0.0.1:GAME_PORT,
   envoi d'un paquet, puis deconnexion propre.
   A lancer avec ./build/server dans un autre terminal. */
static void test_enet(void)
{
    ENetHost *client = enet_host_create(NULL, 1, ENET_CHANNEL_COUNT, 0, 0);
    if (client == NULL)
    {
        fprintf(stderr, "ENet: impossible de creer le host client.\n");
        return;
    }

    ENetAddress address;
    enet_address_set_host(&address, "127.0.0.1");
    address.port = GAME_PORT;

    ENetPeer *peer = enet_host_connect(client, &address, ENET_CHANNEL_COUNT, 0);
    if (peer == NULL)
    {
        fprintf(stderr, "ENet: enet_host_connect a echoue.\n");
        enet_host_destroy(client);
        return;
    }

    ENetEvent event;
    if (enet_host_service(client, &event, 3000) > 0 &&
        event.type == ENET_EVENT_TYPE_CONNECT)
    {
        printf("ENet OK: connecte au serveur 127.0.0.1:%d\n", GAME_PORT);

        ENetPacket *packet = enet_packet_create("ping", 5, ENET_PACKET_FLAG_RELIABLE);
        enet_peer_send(peer, 0, packet);
        enet_host_flush(client);
        printf("ENet OK: paquet 'ping' envoye\n");

        enet_peer_disconnect(peer, 0);
        /* attend l'evenement de deconnexion (ou timeout) */
        while (enet_host_service(client, &event, 1000) > 0 &&
               event.type != ENET_EVENT_TYPE_DISCONNECT)
        {
        }
        printf("ENet OK: deconnecte proprement\n");
    }
    else
    {
        printf("ENet: serveur non atteignable sur 127.0.0.1:%d "
               "(lance ./build/server dans un autre terminal)\n", GAME_PORT);
    }

    enet_host_destroy(client);
}

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    if (enet_initialize() != 0)
    {
        fprintf(stderr, "An error occurred while initializing ENet.\n");
        return EXIT_FAILURE;
    }
    atexit(enet_deinitialize);

    test_enet();

    InitWindow(800, 450, "Game ISEN - test raylib");
    if (!IsWindowReady())
    {
        fprintf(stderr, "raylib: impossible d'ouvrir la fenetre.\n");
        return EXIT_FAILURE;
    }
    SetTargetFPS(60);

    const int screenWidth = 800;
    const int screenHeight = 450;

    // Position initiale de notre cercle (au centre)
    float cercleX = (float)screenWidth / 2;
    float cercleY = (float)screenHeight / 2;
    float vitesse = 5.0f; // Vitesse de déplacement

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        // --- MISE À JOUR ---
        
        if (IsKeyDown(KEY_W)) {
            cercleY -= vitesse; //up
        }
        if (IsKeyDown(KEY_S)) {
            cercleY += vitesse; //down
        }
        if (IsKeyDown(KEY_A)) {
            cercleX -= vitesse; //left
        }
        if (IsKeyDown(KEY_D)) {
            cercleX += vitesse; //right
        }
        // --- DESSIN ---
        BeginDrawing();
            ClearBackground(RAYWHITE);            
            // On dessine le cercle avec ses coordonnées dynamiques
            DrawCircle((int)cercleX, (int)cercleY, 30, MAROON);
            
        EndDrawing();
    }

    CloseWindow();
    printf("raylib OK: fenetre ouverte puis fermee proprement\n");
    return EXIT_SUCCESS;
}
/* 
    // Cursor-related functions
    void ShowCursor(void);                                      // Show cursor
    void HideCursor(void);                                      // Hide cursor
    bool IsCursorHidden(void);                                  // Check if cursor is not visible
    void EnableCursor(void);                                    // Enable cursor (unlock cursor)
    void DisableCursor(void);                                   // Disable cursor (lock cursor)
    bool IsCursorOnScreen(void);                                // Check if cursor is on the screen

    //------------------------------------------------------------------------------------
    // Input Handling Functions (Module: core)
    //------------------------------------------------------------------------------------

    // Input-related functions: keyboard
    bool IsKeyPressed(int key);                             // Check if key has been pressed once
    bool IsKeyPressedRepeat(int key);                       // Check if key has been pressed again
    bool IsKeyDown(int key);                                // Check if key is being pressed
    bool IsKeyReleased(int key);                            // Check if key has been released once
    bool IsKeyUp(int key);                                  // Check if key is NOT being pressed
    int GetKeyPressed(void);                                // Get key pressed (keycode), call it multiple times for keys queued, returns 0 when the queue is empty
    int GetCharPressed(void);                               // Get char pressed (unicode), call it multiple times for chars queued, returns 0 when the queue is empty
    const char *GetKeyName(int key);                        // Get name of a QWERTY key on the current keyboard layout (eg returns string 'q' for KEY_A on an AZERTY keyboard)
    void SetExitKey(int key);                               // Set a custom key to exit program (default is ESC)

    // Input-related functions: gamepads
    bool IsGamepadAvailable(int gamepad);                   // Check if gamepad is available
    const char *GetGamepadName(int gamepad);                // Get gamepad internal name id
    bool IsGamepadButtonPressed(int gamepad, int button);   // Check if gamepad button has been pressed once
    bool IsGamepadButtonDown(int gamepad, int button);      // Check if gamepad button is being pressed
    bool IsGamepadButtonReleased(int gamepad, int button);  // Check if gamepad button has been released once
    bool IsGamepadButtonUp(int gamepad, int button);        // Check if gamepad button is NOT being pressed
    int GetGamepadButtonPressed(void);                      // Get the last gamepad button pressed
    int GetGamepadAxisCount(int gamepad);                   // Get axis count for a gamepad
    float GetGamepadAxisMovement(int gamepad, int axis);    // Get movement value for a gamepad axis
    int SetGamepadMappings(const char *mappings);           // Set internal gamepad mappings (SDL_GameControllerDB)
    void SetGamepadVibration(int gamepad, float leftMotor, float rightMotor, float duration); // Set gamepad vibration for both motors (duration in seconds)

    // Input-related functions: mouse
    bool IsMouseButtonPressed(int button);                  // Check if mouse button has been pressed once
    bool IsMouseButtonDown(int button);                     // Check if mouse button is being pressed
    bool IsMouseButtonReleased(int button);                 // Check if mouse button has been released once
    bool IsMouseButtonUp(int button);                       // Check if mouse button is NOT being pressed
    int GetMouseX(void);                                    // Get mouse position X
    int GetMouseY(void);                                    // Get mouse position Y
    Vector2 GetMousePosition(void);                         // Get mouse position XY
    Vector2 GetMouseDelta(void);                            // Get mouse delta between frames
    void SetMousePosition(int x, int y);                    // Set mouse position XY
    void SetMouseOffset(int offsetX, int offsetY);          // Set mouse offset
    void SetMouseScale(float scaleX, float scaleY);         // Set mouse scaling
    float GetMouseWheelMove(void);                          // Get mouse wheel movement for X or Y, whichever is larger
    Vector2 GetMouseWheelMoveV(void);                       // Get mouse wheel movement for both X and Y
    void SetMouseCursor(int cursor);                        // Set mouse cursor

    // Input-related functions: touch
    int GetTouchX(void);                                    // Get touch position X for touch point 0 (relative to screen size)
    int GetTouchY(void);                                    // Get touch position Y for touch point 0 (relative to screen size)
    Vector2 GetTouchPosition(int index);                    // Get touch position XY for a touch point index (relative to screen size)
    int GetTouchPointId(int index);                         // Get touch point identifier for provided index
    int GetTouchPointCount(void);                           // Get number of touch points

    //------------------------------------------------------------------------------------
    // Gestures and Touch Handling Functions (Module: rgestures)
    //------------------------------------------------------------------------------------
    void SetGesturesEnabled(unsigned int flags);            // Enable a set of gestures using flags
    bool IsGestureDetected(unsigned int gesture);           // Check if gesture has been detected
    int GetGestureDetected(void);                           // Get latest detected gesture
    float GetGestureHoldDuration(void);                     // Get gesture hold time in seconds
    Vector2 GetGestureDragVector(void);                     // Get gesture drag vector
    float GetGestureDragAngle(void);                        // Get gesture drag angle
    Vector2 GetGesturePinchVector(void);                    // Get gesture pinch delta
    float GetGesturePinchAngle(void);                       // Get gesture pinch angle

    //------------------------------------------------------------------------------------
    // Camera System Functions (Module: rcamera)
    //------------------------------------------------------------------------------------
    void UpdateCamera(Camera *camera, int mode);            // Update camera position for selected mode
    void UpdateCameraPro(Camera *camera, Vector3 movement, Vector3 rotation, float zoom); // Update camera movement/rotation
*/