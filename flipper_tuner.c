#include <furi.h>
#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <gui/modules/widget.h>
#include <gui/modules/submenu.h>
#include <gui/modules/text_input.h>
#include <furi_hal.h>
#include "notes.h"
#include <string.h>

#define TAG "flipper_tuner_app"

/* generated by fbt from .png files in images folder */
#include <flipper_tuner_icons.h>

//// Enum/struct setups

typedef struct TunerState {
    float volume;
    bool isPlaying;
    NOTE currentNote;
    int currentNoteIndex;
} TunerState;

// Scenes
typedef enum {
    FlipperTunerMainMenuScene,
    FlipperTunerPlayToneScene,
    FlipperTunerMetronomeScene,
    FlipperTunerSceneCount,
} FlipperTunerScene;

// Views
typedef enum {
    FlipperTunerSubmenuView,
    FlipperTunerWidgetView,
    FlipperTunerTextInputView,
    FlipperTunerPlayToneView,
} FlipperTunerView;

// App object struct
typedef struct App {
    SceneManager* scene_manager;
    ViewDispatcher* view_dispatcher;
    Submenu* submenu;
    Widget* widget;
    TextInput* text_input;
    View* play_tone_view;
    TunerState* tuner_state;
} App;

typedef enum {
    FlipperTunerMainMenuScenePlayTone,
    FlipperTunerMainMenuSceneMetronome,
} FlipperTunerMainMenuSceneIndex;

typedef enum {
    FlipperTunerMainMenuScenePlayToneEvent,
    FlipperTunerMainMenuSceneMetronomeEvent,
} FlipperTunerMainMenuEvent;

void flipper_tuner_menu_callback(void* context, uint32_t index) {
    App* app = context;
    switch(index) {
    case FlipperTunerMainMenuScenePlayTone:
        scene_manager_handle_custom_event(
            app->scene_manager, FlipperTunerMainMenuScenePlayToneEvent);
        break;
    case FlipperTunerMainMenuSceneMetronome:
        scene_manager_handle_custom_event(
            app->scene_manager, FlipperTunerMainMenuSceneMetronomeEvent);
        break;
    }
}

// Main Menu scene
void flipper_tuner_main_menu_scene_on_enter(void* context) {
    App* app = context;
    submenu_reset(app->submenu); // Reset submenu
    submenu_set_header(app->submenu, "Flipper Tuner"); // Set header
    submenu_add_item(
        app->submenu,
        "Play Tone",
        FlipperTunerMainMenuScenePlayTone,
        flipper_tuner_menu_callback,
        app);
    submenu_add_item(
        app->submenu,
        "Metronome",
        FlipperTunerMainMenuSceneMetronome,
        flipper_tuner_menu_callback,
        app);
    view_dispatcher_switch_to_view(app->view_dispatcher, FlipperTunerSubmenuView);
}
bool flipper_tuner_main_menu_scene_on_event(void* context, SceneManagerEvent event) {
    App* app = context;
    bool consumed = false;

    switch(event.type) {
    case SceneManagerEventTypeCustom:
        switch(event.event) {
        case FlipperTunerMainMenuScenePlayToneEvent:
            scene_manager_next_scene(app->scene_manager, FlipperTunerPlayToneScene);
            consumed = true;
            break;
        case FlipperTunerMainMenuSceneMetronomeEvent:
            scene_manager_next_scene(app->scene_manager, FlipperTunerMetronomeScene);
            consumed = true;
            break;
        }
        break;
    default:
        break;
    }
    return consumed;
}
void flipper_tuner_main_menu_scene_on_exit(void* context) {
    App* app = context;
    submenu_reset(app->submenu);
}

void play(TunerState* tunerState) {
    if(furi_hal_speaker_is_mine() || furi_hal_speaker_acquire(1000)) {
        furi_hal_speaker_start(tunerState->currentNote.frequency, tunerState->volume);
        tunerState->isPlaying = true;
    }
}
void stop(TunerState* tunerState) {
    if(furi_hal_speaker_is_mine()) {
        furi_hal_speaker_stop();
        furi_hal_speaker_release();
        tunerState->isPlaying = false;
    }
}

void increase_volume(TunerState* tunerState) {
    if(tunerState->volume < 1.0f) {
        tunerState->volume += 0.1f;
    }
}

void decrease_volume(TunerState* tunerState) {
    if(tunerState->volume > 0.0f) {
        tunerState->volume -= 0.1f;
    }
}

void increase_frequency(TunerState* tunerState) {
    if(((unsigned int)tunerState->currentNoteIndex) + 1 < (sizeof(tunings) / sizeof(tunings[0]))) {
        tunerState->currentNoteIndex += 1;
        tunerState->currentNote = tunings[tunerState->currentNoteIndex];
    }
}

void decrease_frequency(TunerState* tunerState) {
    if(tunerState->currentNoteIndex - 1 >= 0) {
        tunerState->currentNoteIndex -= 1;
        tunerState->currentNote = tunings[tunerState->currentNoteIndex];
    }
}

static void current_note(TunerState* tunerState, char* outLabel) {
    if(outLabel) {
        strncpy(outLabel, tunerState->currentNote.label, 8);
    }
}

// Play tone scene
static bool play_tone_input_callback(InputEvent* event, void* context) {
    TunerState* tunerState = context;
    bool consumed = false;

    if(event->type == InputTypeShort) {
        // Stop/start
        switch(event->key) {
        case InputKeyOk:
            if(!tunerState->isPlaying) {
                play(tunerState);
            } else {
                stop(tunerState);
            }
            break;
        case InputKeyUp:
            increase_volume(tunerState);
            if(tunerState->isPlaying) {
                stop(tunerState);
                play(tunerState);
            }
            break;
        case InputKeyDown:
            decrease_volume(tunerState);
            if(tunerState->isPlaying) {
                stop(tunerState);
                play(tunerState);
            }
            break;
        case InputKeyLeft:
            decrease_frequency(tunerState);
            if(tunerState->isPlaying) {
                stop(tunerState);
                play(tunerState);
            }
            break;
        case InputKeyRight:
            increase_frequency(tunerState);
            if(tunerState->isPlaying) {
                stop(tunerState);
                play(tunerState);
            }
            break;
        case InputKeyBack:
            stop(tunerState);
        default:
            break;
        }
        FURI_LOG_I(TAG, "ok key pressed");
        consumed = true;
    }
    return consumed;
}

void play_tone_view_draw_callback(Canvas* canvas, void* model) {
    TunerState* tunerState = model;
    assert(tunerState);

    canvas_set_font(canvas, FontPrimary);
    FuriString* note = furi_string_alloc();
    char noteLabel[8];
    current_note(tunerState, noteLabel);
    furi_string_printf(note, "< %s >", noteLabel); // FIXME: noteLabel not displaying anything
    canvas_draw_str_aligned(canvas, 20, 10, AlignCenter, AlignCenter, furi_string_get_cstr(note));
    furi_string_free(note);
}

void flipper_tuner_play_tone_scene_on_enter(void* context) {
    App* app = context;
    view_dispatcher_switch_to_view(app->view_dispatcher, FlipperTunerPlayToneView);
}
bool flipper_tuner_play_tone_scene_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    return false;
}
void flipper_tuner_play_tone_scene_on_exit(void* context) {
    UNUSED(context);
}

void flipper_tuner_metronome_scene_on_enter(void* context) {
    UNUSED(context);
}
bool flipper_tuner_metronome_scene_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    return false;
}
void flipper_tuner_metronome_scene_on_exit(void* context) {
    UNUSED(context);
}

/// handler arrays must be in same order as scenes enum
// on_enter handlers
void (*const flipper_tuner_scene_on_enter_handlers[])(void*) = {
    flipper_tuner_main_menu_scene_on_enter,
    flipper_tuner_play_tone_scene_on_enter,
    flipper_tuner_metronome_scene_on_enter,
};

// on_event handlers
bool (*const flipper_tuner_scene_on_event_handlers[])(void*, SceneManagerEvent) = {
    flipper_tuner_main_menu_scene_on_event,
    flipper_tuner_play_tone_scene_on_event,
    flipper_tuner_metronome_scene_on_event,
};

// on_exit handlers
void (*const flipper_tuner_scene_on_exit_handlers[])(void*) = {
    flipper_tuner_main_menu_scene_on_exit,
    flipper_tuner_play_tone_scene_on_exit,
    flipper_tuner_metronome_scene_on_exit,
};

// scene manager handler object
static const SceneManagerHandlers flipper_tuner_scene_manager_handlers = {
    .on_enter_handlers = flipper_tuner_scene_on_enter_handlers,
    .on_event_handlers = flipper_tuner_scene_on_event_handlers,
    .on_exit_handlers = flipper_tuner_scene_on_exit_handlers,
    .scene_num = FlipperTunerSceneCount,
};

// Handles custom events like timer, GPIO pins
static bool basic_scene_custom_callback(void* context, uint32_t custom_event) {
    furi_assert(context);
    App* app = context;
    return scene_manager_handle_custom_event(app->scene_manager, custom_event);
}

bool basic_scene_back_event_callback(void* context) {
    furi_assert(context);
    App* app = context;
    return scene_manager_handle_back_event(app->scene_manager);
}

// Memory allocation for app
static App* app_alloc() {
    App* app = malloc(sizeof(App));
    app->scene_manager = scene_manager_alloc(&flipper_tuner_scene_manager_handlers, app);
    app->view_dispatcher = view_dispatcher_alloc();

    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);
    view_dispatcher_set_custom_event_callback(app->view_dispatcher, basic_scene_custom_callback);
    view_dispatcher_set_navigation_event_callback(
        app->view_dispatcher, basic_scene_back_event_callback);

    // Submenu view
    app->submenu = submenu_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, FlipperTunerSubmenuView, submenu_get_view(app->submenu));

    // Widget view
    app->widget = widget_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, FlipperTunerWidgetView, widget_get_view(app->widget));

    // Text input view
    app->text_input = text_input_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, FlipperTunerTextInputView, text_input_get_view(app->text_input));

    // Play Tone view
    app->play_tone_view = view_alloc();

    app->tuner_state = malloc(sizeof(TunerState));
    app->tuner_state->currentNoteIndex = 55; // A4
    app->tuner_state->currentNote = tunings[app->tuner_state->currentNoteIndex];
    app->tuner_state->volume = 1.0f;
    app->tuner_state->isPlaying = false;

    view_set_context(app->play_tone_view, app->tuner_state); // FIXME: not working??
    view_set_draw_callback(app->play_tone_view, play_tone_view_draw_callback);
    view_set_input_callback(app->play_tone_view, play_tone_input_callback);
    view_dispatcher_add_view(app->view_dispatcher, FlipperTunerPlayToneView, app->play_tone_view);

    view_allocate_model(app->play_tone_view, ViewModelTypeLockFree, sizeof(TunerState));

    return app;
}

static void app_free(App* app) {
    furi_assert(app);

    // Remove views
    view_dispatcher_remove_view(app->view_dispatcher, FlipperTunerSubmenuView);
    view_dispatcher_remove_view(app->view_dispatcher, FlipperTunerWidgetView);
    view_dispatcher_remove_view(app->view_dispatcher, FlipperTunerTextInputView);
    view_dispatcher_remove_view(app->view_dispatcher, FlipperTunerPlayToneView);

    // Memory free
    scene_manager_free(app->scene_manager);
    view_dispatcher_free(app->view_dispatcher);
    submenu_free(app->submenu);
    widget_free(app->widget);
    text_input_free(app->text_input);
    view_free(app->play_tone_view);
    free(app->tuner_state);
    free(app);
}

int32_t flipper_tuner_app(void* p) {
    UNUSED(p);
    App* app = app_alloc();

    Gui* gui = furi_record_open(RECORD_GUI);
    view_dispatcher_attach_to_gui(
        app->view_dispatcher, gui, ViewDispatcherTypeFullscreen); // Initialize GUI
    scene_manager_next_scene(app->scene_manager, FlipperTunerMainMenuScene); // Begin at main menu
    view_dispatcher_run(app->view_dispatcher);

    app_free(app);
    return 0;
}

// Scenes enum
// Views enum
// index enum??
// event enums??
// menu callback function
// _on_enter, _on_event, _on_exit functions for each scene
// _on_enter_handlers, _on_event_handlers, _on_exit_handlers arrays
// SceneManagerHandlers object
// _custom_callback function
// _back_event_callback function
// app_alloc function
// app_free function
// app function
