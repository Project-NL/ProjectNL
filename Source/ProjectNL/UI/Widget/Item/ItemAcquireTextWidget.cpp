#include "ItemAcquireTextWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/TextBlock.h"


void UItemAcquireTextWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 1) CanvasPanel을 RootWidget으로 가져오기 (UMG 디자이너에서 설정된 경우)
	UCanvasPanel* RootCanvas = Cast<UCanvasPanel>(GetRootWidget());
	if (!RootCanvas)
	{
		// RootWidget이 CanvasPanel이 아닌 경우, 새로 생성
		RootCanvas = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass());
		WidgetTree->RootWidget = RootCanvas;
	}

	// 2) TextBlock 생성
	UTextBlock* AcquireText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("AcquireText"));
	AcquireText->SetText(FText::FromString(TEXT("Press E to Acquire")));
	AcquireText->SetJustification(ETextJustify::Center);

	// 3) TextBlock을 CanvasPanel에 추가하고 위치 설정
	UCanvasPanelSlot* CanvasSlot = RootCanvas->AddChildToCanvas(AcquireText);
	if (CanvasSlot)
	{
		CanvasSlot->SetPosition(FVector2D(0.0f, 0.0f)); // 텍스트 위치 조정 (필요에 따라 변경)
		CanvasSlot->SetSize(FVector2D(200.0f, 50.0f)); // 텍스트 크기 조정 (필요에 따라 변경)
	}
}
