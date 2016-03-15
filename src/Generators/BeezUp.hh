<?hh // strict

namespace ElasticExport\Generators;

use Plenty\Modules\DataExchange\Contracts\CSVGenerator;
use Plenty\Modules\Helper\Services\ArrayHelper;
use Plenty\Modules\Item\DataLayer\Models\Record;
use Plenty\Modules\Item\DataLayer\Models\RecordList;
use Plenty\Modules\DataExchange\Models\FormatSetting;
use ElasticExport\Helper\ElasticExportHelper;
use Plenty\Modules\Helper\Models\KeyValue;
use Plenty\Modules\Item\Attribute\Contracts\AttributeValueLangRepositoryContract;
use Plenty\Modules\Item\Attribute\Models\AttributeValueLang;
use Plenty\Modules\Character\Contracts\CharacterSelectionRepositoryContract;
use Plenty\Modules\Character\Models\CharacterSelection;

class BeezUp extends CSVGenerator
{
    const string CHARACTER_TYPE_DESCRIPTION					= 'description';
        /*
         * @var ElasticExportHelper
         */
    private ElasticExportHelper $elasticExportHelper;

        /*
         * @var ArrayHelper
         */
    private ArrayHelper $arrayHelper;

        /**
         * AttributeValueLangRepositoryContract $attributeValueLangRepository
         */
    private AttributeValueLangRepositoryContract $attributeValueLangRepository;

        /**
         * CharacterSelectionRepositoryContract $characterSelectionRepository
         */
    private CharacterSelectionRepositoryContract $characterSelectionRepository;

        /**
         * @var array<int,mixed>
         */
    private array<int, array<string, string>>$itemPropertyCache = [];


    /**
     * Geizhals constructor.
     * @param ElasticExportHelper $elasticExportHelper
     * @param ArrayHelper $arrayHelper
     * @param AttributeValueLangRepositoryContract $attributeValueLangRepository
     * @param CharacterSelectionRepositoryContract $characterSelectionRepository
     */
    public function __construct(
        ElasticExportHelper $elasticExportHelper,
        ArrayHelper $arrayHelper,
        AttributeValueLangRepositoryContract $attributeValueLangRepository,
        CharacterSelectionRepositoryContract $characterSelectionRepository
    )
    {
        $this->elasticExportHelper = $elasticExportHelper;
        $this->arrayHelper = $arrayHelper;
        $this->attributeValueLangRepository = $attributeValueLangRepository;
        $this->characterSelectionRepository = $characterSelectionRepository;
    }

    /**
     * @param mixed $resultData
     */
    protected function generateContent(mixed $resultData, array<FormatSetting> $formatSettings = []):void
	{
		if($resultData instanceof RecordList)
		{
			$settings = $this->arrayHelper->buildMapFromObjectList($formatSettings, 'key', 'value');

			$this->setDelimiter(";");

			$this->addCSVContent([
                'Produkt ID',
                'Artikel Nr',
                'MPN',
                'EAN',
                'Marke',
                'Produktname',
                'Produktbeschreibung',
                'Preis inkl. MwSt.',
                'UVP inkl. MwSt.',
                'Produkt-URL',
                'Bild-URL',
                'Bild-URL2',
                'Bild-URL3',
                'Bild-URL4',
                'Bild-URL5',
                'Lieferkosten',
                'Auf Lager',
                'Lagerbestand',
                'Lieferfrist',
                'Kategorie 1',
                'Kategorie 2',
                'Kategorie 3',
                'Kategorie 4',
                'Kategorie 5',
                'Kategorie 6',
                'Farbe',
                'Größe',
                'Gewicht',
                'Grundpreis',
                'ID'
			]);

            $rows = [];

			foreach($resultData as $item)
			{
                $variationAttributes = $this->getVariationAttributes($item, $settings);
                $rrp = $item->variationRecommendedRetailPrice->price > $this->elasticExportHelper->getPrice($item, $settings) ? $item->variationRecommendedRetailPrice->price : $this->elasticExportHelper->getPrice($item, $settings);

				$data = [
					'Produkt ID'            =>  $item->itemBase->id,
                    'Artikel Nr'            =>  $item->variationBase->customNumber,
                    'MPN'                   =>  $item->variationBase->model,
                    'EAN'                   =>  $item->variationBarcode->code,
                    'Marke'                 =>  $item->itemBase->producer,
                    'Produktname'           =>  $this->elasticExportHelper->getName($item, $settings, 256),
                    'Produktbeschreibung'   =>  $this->getDescription($item, $settings),
                    'Preis inkl. MwSt.'     =>  number_format($this->elasticExportHelper->getPrice($item, $settings), 2, '.', '')  ,
                    'UVP inkl. MwSt.'       =>  number_format($rrp, 2, '.', ''),
                    'Produkt-URL'           =>  $this->elasticExportHelper->getUrl($item, $settings),
                    'Bild-URL'              =>  $this->getImageByNumber($item, $settings, 1),
                    'Bild-URL2'             =>  $this->getImageByNumber($item, $settings, 2),
                    'Bild-URL3'             =>  $this->getImageByNumber($item, $settings, 3),
                    'Bild-URL4'             =>  $this->getImageByNumber($item, $settings, 4),
                    'Bild-URL5'             =>  $this->getImageByNumber($item, $settings, 5),
                    'Lieferkosten'          =>  number_format($this->elasticExportHelper->getShippingCost($item, $settings), 2, ',', ''),
                    'Auf Lager'             =>  $item->variationStock->stockNet > 0 ? 'Y' : 'N',
                    'Lagerbestand'          =>  $item->variationStock->stockNet,
                    'Lieferfrist'           =>  $this->elasticExportHelper->getAvailability($item, $settings, false),
                    'Kategorie 1'           =>  $this->elasticExportHelper->getCategoryBranch($item, $settings, 1),
                    'Kategorie 2'           =>  $this->elasticExportHelper->getCategoryBranch($item, $settings, 2),
                    'Kategorie 3'           =>  $this->elasticExportHelper->getCategoryBranch($item, $settings, 3),
                    'Kategorie 4'           =>  $this->elasticExportHelper->getCategoryBranch($item, $settings, 4),
                    'Kategorie 5'           =>  $this->elasticExportHelper->getCategoryBranch($item, $settings, 5),
                    'Kategorie 6'           =>  $this->elasticExportHelper->getCategoryBranch($item, $settings, 6),
                    'Farbe'                 =>  $variationAttributes['Color'],
                    'Größe'                 =>  $variationAttributes['Size'],
                    'Gewicht'               =>  $item->variationBase->weightG,
                    'Grundpreis'            =>  $this->elasticExportHelper->getBasePrice($item, $settings),
                    'ID'                    =>  '', //TODO
				];

				$this->addCSVContent(array_values($data));
			}
        }
    }

    /**
     * Get item description.
     * @param Record $item
     * @param KeyValue $settings
     * @return string
     */
    private function getDescription(Record $item, KeyValue $settings):string
    {
        $description = $this->elasticExportHelper->getItemCharacterByBackendName($item, $settings, self::CHARACTER_TYPE_DESCRIPTION);

        if (strlen($description) <= 0)
        {
            $description = $this->elasticExportHelper->getDescription($item, $settings, 5000);
        }

        return $description;
    }

    /**
     * Get variation attributes.
     * @param  Record   $item
     * @param  KeyValue $settings
     * @return array<string,string>
     */
    private function getVariationAttributes(Record $item, KeyValue $settings):array<string,string>
    {
        $variationAttributes = [];

        foreach($item->variationAttributeValueList as $variationAttribute)
        {
            $attributeValueLang = $this->attributeValueLangRepository->findAttributeValue($variationAttribute->attributeValueId, $settings->get('lang'));

            if($attributeValueLang instanceof AttributeValueLang)
            {
                if($attributeValueLang->attributeValue->attribute->amazon_variation)
                {
                    $variationAttributes[$attributeValueLang->attributeValue->attribute->amazon_variation][] = $attributeValueLang->name;
                }
            }
        }

        return $variationAttributes;
    }

        /**
         * @param Record $item
         * @param KeyValue $settings
         * @param int $number
         * @return string
         */
    private function getImageByNumber(Record $item, KeyValue $settings, int $number):string
    {
        $imageList = $this->elasticExportHelper->getImageList($item, $settings);

        if(count($imageList) > 0 && array_key_exists($number, $imageList))
        {
            return $imageList[$number];
        }
        else
        {
            return '';
        }
    }
}
