// Copyright @SchGame - 2025. All Rights Reserved.

#include "DoubleClickCast.h"

#include "K2Node_DynamicCast.h"
#include "SGraphNode.h"
#include "SourceCodeNavigation.h"
#include "UnrealEdGlobals.h"
#include "Editor/UnrealEdEngine.h"
#include "KismetNodes/SGraphNodeK2Base.h"
#include "Preferences/UnrealEdOptions.h"

#define LOCTEXT_NAMESPACE "FDoubleClickCastModule"

FDoubleClickCastInputProcessor& FDoubleClickCastInputProcessor::Get()
{
	static FDoubleClickCastInputProcessor Instance;
	return Instance;
}

bool FDoubleClickCastInputProcessor::HandleMouseButtonDoubleClickEvent(FSlateApplication& SlateApp,
                                                                       const FPointerEvent& MouseEvent)
{
    const FVector2D ScreenPos = MouseEvent.GetScreenSpacePosition();
    const TArray<TSharedRef<SWindow>>& TopLevelWindows = FSlateApplication::Get().GetInteractiveTopLevelWindows();
    const int32 UserIndex = MouseEvent.GetUserIndex();
    
    FWidgetPath WidgetsUnderCursor = FSlateApplication::Get().LocateWindowUnderMouse(ScreenPos, TopLevelWindows, false, UserIndex);

    for (int32 i = 0; i < WidgetsUnderCursor.Widgets.Num(); ++i)
    {
        TSharedPtr<SWidget> CurrentWidget = WidgetsUnderCursor.Widgets[i].GetWidgetPtr()->AsShared();
        if (CurrentWidget->GetType() != TEXT("SGraphNodeK2Default"))
            continue;

        const TSharedPtr<SGraphNode> GraphNodeWidget = StaticCastSharedPtr<SGraphNode>(CurrentWidget);
        UEdGraphNode* EdGraphNode = GraphNodeWidget->GetNodeObj();
        const UK2Node_DynamicCast* DynamicCastNode = Cast<UK2Node_DynamicCast>(EdGraphNode);
        
        if (!DynamicCastNode || !DynamicCastNode->TargetType)
            continue;

        const UClass* TargetType = DynamicCastNode->TargetType;

        if (TargetType->IsInBlueprint())
        {
            if (UAssetEditorSubsystem* AssetEditorSubsystem = GUnrealEd->GetEditorSubsystem<UAssetEditorSubsystem>())
            {
                AssetEditorSubsystem->OpenEditorForAsset(TargetType->ClassGeneratedBy);
            }
            continue;
        }

        if (!GUnrealEd || !GUnrealEd->GetUnrealEdOptions()->IsCPPAllowed())
            continue;

        FString NativeHeaderPath;
        const bool bFoundHeader = FSourceCodeNavigation::FindClassHeaderPath(TargetType, NativeHeaderPath);

        if (const bool bHeaderExists = IFileManager::Get().FileSize(*NativeHeaderPath) != INDEX_NONE; bFoundHeader && bHeaderExists)
        {
            const FString AbsPath = FPaths::ConvertRelativePathToFull(NativeHeaderPath);
            FSourceCodeNavigation::OpenSourceFile(AbsPath);
        }
    }

    return IInputProcessor::HandleMouseButtonDoubleClickEvent(SlateApp, MouseEvent);
}



void FDoubleClickCastModule::StartupModule()
{
	InputProcessor = MakeShared<FDoubleClickCastInputProcessor>();
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	FSlateApplication::Get().RegisterInputPreProcessor(InputProcessor.ToSharedRef());
}

void FDoubleClickCastModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FDoubleClickCastModule, DoubleClickCast)