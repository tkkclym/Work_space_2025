```c++
UCLASS()

class UMyDataAsset:public UDataAsset 

{

GENERATED_BODY()
public: 
UPROPERTY(BlueprintReadWrite,EditAnywhere)
TArray<FMyDataStruct>Datas;

}
```

