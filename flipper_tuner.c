#include <furi.h>
#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <gui/modules/widget.h>
#include <gui/modules/submenu.h>
#include <gui/modules/text_input.h>
#include <furi_hal.h>

/* generated by fbt from .png files in images folder */
#include <flipper_tuner_icons.h>

//// Enum/struct setups

// Scenes
typedef enum {
    FlipperTunerMainMenuScene,
    FlipperTunerLotteryScene,
    FlipperTunerGreetingInputScene,
    FlipperTunerGreetingMessageScene,
    FlipperTunerPlayToneScene,
    FlipperTunerSaveTuningScene,
    FlipperTunerLoadTuningScene,
    FlipperTunerSceneCount,
} FlipperTunerScene;

// Views
typedef enum {
    FlipperTunerSubmenuView,
    FlipperTunerWidgetView,
    FlipperTunerTextInputView,
} FlipperTunerView;

// App object struct
typedef struct App {
    SceneManager* scene_manager;
    ViewDispatcher* view_dispatcher;
    Submenu* submenu;
    Widget* widget;
    TextInput* text_input;
    char* user_name;
    uint8_t user_name_size;
} App;

typedef enum {
    FlipperTunerMainMenuSceneLottoNumbers,
    FlipperTunerMainMenuSceneGreeting,
    FlipperTunerMainMenuScenePlayTone,
    FlipperTunerMainMenuSceneSaveTuning,
    FlipperTunerMainMenuSceneLoadTuning,
} FlipperTunerMainMenuSceneIndex;

typedef enum {
    FlipperTunerMainMenuSceneLottoNumbersEvent,
    FlipperTunerMainMenuSceneGreetingEvent,
    FlipperTunerMainMenuScenePlayToneEvent,
    FlipperTunerMainMenuSceneSaveTuningEvent,
    FlipperTunerMainMenuSceneLoadTuningEvent,
} FlipperTunerMainMenuEvent;

typedef enum {
    FlipperTunerGreetingInputSceneSaveEvent,
} FlipperTunerGreetingInputEvent;

bool isPlaying;

//// Enter, event & exit functions for all scenes

void flipper_tuner_menu_callback(void* context, uint32_t index) {
    App* app = context;
    switch(index) {
    case FlipperTunerMainMenuSceneLottoNumbers:
        scene_manager_handle_custom_event(
            app->scene_manager, FlipperTunerMainMenuSceneLottoNumbersEvent);
        break;
    case FlipperTunerMainMenuSceneGreeting:
        scene_manager_handle_custom_event(
            app->scene_manager, FlipperTunerMainMenuSceneGreetingEvent);
        break;
    case FlipperTunerMainMenuScenePlayTone:
        scene_manager_handle_custom_event(
            app->scene_manager, FlipperTunerMainMenuScenePlayToneEvent);
        break;
    case FlipperTunerMainMenuSceneSaveTuning:
        scene_manager_handle_custom_event(
            app->scene_manager, FlipperTunerMainMenuSceneSaveTuningEvent);
        break;
    case FlipperTunerMainMenuSceneLoadTuning:
        scene_manager_handle_custom_event(
            app->scene_manager, FlipperTunerMainMenuSceneLoadTuningEvent);
        break;
    }
}

void flipper_tuner_main_menu_scene_on_enter(void* context) {
    App* app = context;
    submenu_reset(app->submenu); // Reset submenu
    submenu_set_header(app->submenu, "Basic Scenes Demo"); // Set header
    submenu_add_item(
        app->submenu,
        "Lotto Numbers",
        FlipperTunerMainMenuSceneLottoNumbers,
        flipper_tuner_menu_callback,
        app); // Add Lotto Numbers menu item
    submenu_add_item(
        app->submenu,
        "Greeting",
        FlipperTunerMainMenuSceneGreeting,
        flipper_tuner_menu_callback,
        app); // Add Greeting menu item
    submenu_add_item(
        app->submenu,
        "Play Tone",
        FlipperTunerMainMenuScenePlayTone,
        flipper_tuner_menu_callback,
        app);
    submenu_add_item(
        app->submenu,
        "Save New Tuning",
        FlipperTunerMainMenuSceneSaveTuning,
        flipper_tuner_menu_callback,
        app);
    submenu_add_item(
        app->submenu,
        "Load Tuning",
        FlipperTunerMainMenuSceneLoadTuning,
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
        case FlipperTunerMainMenuSceneLottoNumbersEvent:
            scene_manager_next_scene(app->scene_manager, FlipperTunerLotteryScene);
            consumed = true;
            break;
        case FlipperTunerMainMenuSceneGreetingEvent:
            scene_manager_next_scene(app->scene_manager, FlipperTunerGreetingInputScene);
            consumed = true;
            break;
        case FlipperTunerMainMenuScenePlayToneEvent:
            scene_manager_next_scene(app->scene_manager, FlipperTunerPlayToneScene);
            consumed = true;
            break;
        case FlipperTunerMainMenuSceneSaveTuningEvent:
            scene_manager_next_scene(app->scene_manager, FlipperTunerSaveTuningScene);
            consumed = true;
            break;
        case FlipperTunerMainMenuSceneLoadTuningEvent:
            scene_manager_next_scene(app->scene_manager, FlipperTunerLoadTuningScene);
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

void flipper_tuner_lottery_scene_on_enter(void* context) {
    App* app = context;
    widget_reset(app->widget);
    widget_add_string_element(
        app->widget, 25, 15, AlignLeft, AlignCenter, FontPrimary, "Lotto Numbers:");
    widget_add_string_element(
        app->widget, 30, 35, AlignLeft, AlignCenter, FontBigNumbers, "0 4 2");
    view_dispatcher_switch_to_view(app->view_dispatcher, FlipperTunerWidgetView);
}
bool flipper_tuner_lottery_scene_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    return false; // event not handled
}
void flipper_tuner_lottery_scene_on_exit(void* context) {
    UNUSED(context);
}

void flipper_tuner_text_input_callback(void* context) {
    App* app = context;
    scene_manager_handle_custom_event(app->scene_manager, FlipperTunerGreetingInputSceneSaveEvent);
}
void flipper_tuner_greeting_input_scene_on_enter(void* context) {
    App* app = context;
    bool clear_text = true;

    text_input_reset(app->text_input);
    text_input_set_header_text(app->text_input, "Enter your name");
    text_input_set_result_callback(
        app->text_input,
        flipper_tuner_text_input_callback,
        app,
        app->user_name,
        app->user_name_size,
        clear_text);
    view_dispatcher_switch_to_view(app->view_dispatcher, FlipperTunerTextInputView);
}
bool flipper_tuner_greeting_input_scene_on_event(void* context, SceneManagerEvent event) {
    App* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == FlipperTunerGreetingInputSceneSaveEvent) {
            scene_manager_next_scene(app->scene_manager, FlipperTunerGreetingMessageScene);
            consumed = true;
        }
    }

    return consumed;
}
void flipper_tuner_greeting_input_scene_on_exit(void* context) {
    UNUSED(context);
}

void flipper_tuner_greeting_message_scene_on_enter(void* context) {
    App* app = context;
    widget_reset(app->widget);
    FuriString* message = furi_string_alloc();
    furi_string_printf(message, "Hello,\n%s!", app->user_name);
    widget_add_string_multiline_element(
        app->widget, 5, 15, AlignLeft, AlignCenter, FontPrimary, furi_string_get_cstr(message));
    furi_string_free(message);
    view_dispatcher_switch_to_view(app->view_dispatcher, FlipperTunerWidgetView);
}
bool flipper_tuner_greeting_message_scene_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    return false; // event not handled
}
void flipper_tuner_greeting_message_scene_on_exit(void* context) {
    App* app = context;
    widget_reset(app->widget);
}

void play() {
    if(furi_hal_speaker_is_mine() || furi_hal_speaker_acquire(1000)) {
        furi_hal_speaker_start(65.41, 50);
    }
}
void stop() {
    if(furi_hal_speaker_is_mine()) {
        furi_hal_speaker_stop();
        furi_hal_speaker_release();
    }
}

void flipper_tuner_play_tone_scene_on_enter(void* context) {
    App* app = context;
    widget_reset(app->widget);
    widget_add_string_element(
        app->widget, 25, 15, AlignLeft, AlignCenter, FontPrimary, "Play Tone");
    view_dispatcher_switch_to_view(app->view_dispatcher, FlipperTunerWidgetView);
}
bool flipper_tuner_play_tone_scene_on_event(void* context, SceneManagerEvent event) {
    App* app = context;
    UNUSED(event); // TODO: finish play tone implementation
    return false;
}
void flipper_tuner_play_tone_scene_on_exit(void* context) {
    UNUSED(context);
}

void flipper_tuner_save_tuning_scene_on_enter(void* context) {
    UNUSED(context);
}
bool flipper_tuner_save_tuning_scene_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    return false;
}
void flipper_tuner_save_tuning_scene_on_exit(void* context) {
    UNUSED(context);
}

void flipper_tuner_load_tuning_scene_on_enter(void* context) {
    UNUSED(context);
}
bool flipper_tuner_load_tuning_scene_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    return false;
}
void flipper_tuner_load_tuning_scene_on_exit(void* context) {
    UNUSED(context);
}

/// handler arrays must be in same order as scenes enum
// on_enter handlers
void (*const flipper_tuner_scene_on_enter_handlers[])(void*) = {
    flipper_tuner_main_menu_scene_on_enter,
    flipper_tuner_lottery_scene_on_enter,
    flipper_tuner_greeting_input_scene_on_enter,
    flipper_tuner_greeting_message_scene_on_enter,
    flipper_tuner_play_tone_scene_on_enter,
    flipper_tuner_save_tuning_scene_on_enter,
    flipper_tuner_load_tuning_scene_on_enter,
};

// on_event handlers
bool (*const flipper_tuner_scene_on_event_handlers[])(void*, SceneManagerEvent) = {
    flipper_tuner_main_menu_scene_on_event,
    flipper_tuner_lottery_scene_on_event,
    flipper_tuner_greeting_input_scene_on_event,
    flipper_tuner_greeting_message_scene_on_event,
    flipper_tuner_play_tone_scene_on_event,
    flipper_tuner_save_tuning_scene_on_event,
    flipper_tuner_load_tuning_scene_on_event,
};

// on_exit handlers
void (*const flipper_tuner_scene_on_exit_handlers[])(void*) = {
    flipper_tuner_main_menu_scene_on_exit,
    flipper_tuner_lottery_scene_on_exit,
    flipper_tuner_greeting_input_scene_on_exit,
    flipper_tuner_greeting_message_scene_on_exit,
    flipper_tuner_play_tone_scene_on_exit,
    flipper_tuner_save_tuning_scene_on_exit,
    flipper_tuner_load_tuning_scene_on_exit,
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
    app->user_name_size = 16;
    app->user_name = malloc(app->user_name_size);

    //view_dispatcher_enable_queue(app->view_dispatcher);  DEPRECATED, on by default
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

    return app;
}

static void app_free(App* app) {
    furi_assert(app);

    // Remove views
    view_dispatcher_remove_view(app->view_dispatcher, FlipperTunerSubmenuView);
    view_dispatcher_remove_view(app->view_dispatcher, FlipperTunerWidgetView);
    view_dispatcher_remove_view(app->view_dispatcher, FlipperTunerTextInputView);

    // Memory free
    scene_manager_free(app->scene_manager);
    view_dispatcher_free(app->view_dispatcher);
    submenu_free(app->submenu);
    widget_free(app->widget);
    text_input_free(app->text_input);
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
    isPlaying = false;

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
