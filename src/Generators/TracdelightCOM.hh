<?hh // strict
namespace ElasticExport\Generators;

use Plenty\Modules\DataExchange\Contracts\CSVGenerator;
use Plenty\Modules\Helper\Services\ArrayHelper;
use Plenty\Modules\Item\DataLayer\Models\Record;
use Plenty\Modules\Item\DataLayer\Models\RecordList;
use Plenty\Modules\DataExchange\Models\FormatSetting;
use ElasticExport\Helper\ElasticExportHelper;
use Plenty\Modules\Item\Attribute\Contracts\AttributeValueLangRepositoryContract;
use Plenty\Modules\Item\Attribute\Models\AttributeValueLang;
use Plenty\Modules\Helper\Models\KeyValue;
use Plenty\Modules\Character\Contracts\CharacterSelectionRepositoryContract;
use Plenty\Modules\Character\Models\CharacterSelection;

class TracdelightCOM extends CSVGenerator
{
    const string DELIMITER = ';';
    /*
     * @var ElasticExportHelper
     */
    private ElasticExportHelper $elasticExportHelper;

    /**
     * AttributeValueLangRepositoryContract $attributeValueLangRepository
     */
    private AttributeValueLangRepositoryContract $attributeValueLangRepository;

    /**
     * CharacterSelectionRepositoryContract $characterSelectionRepository
     */
    private CharacterSelectionRepositoryContract $characterSelectionRepository;

    /*
     * @var ArrayHelper
     */
    private ArrayHelper $arrayHelper;

    /**
     * @var array<int,mixed>
     */
    private array<int,array<string,string>>$itemPropertyCache = [];

    /**
     * TracdelightCOM constructor.
     * @param ElasticExportHelper $elasticExportHelper
     * @param ArrayHelper $arrayHelper
     */
    public function __construct(ElasticExportHelper $elasticExportHelper,
                                ArrayHelper $arrayHelper,
                                AttributeValueLangRepositoryContract $attributeValueLangRepository,
                                CharacterSelectionRepositoryContract $characterSelectionRepository)
    {
        $this->elasticExportHelper          = $elasticExportHelper;
        $this->arrayHelper                  = $arrayHelper;
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

			$this->setDelimiter(self::DELIMITER);

			$this->addCSVContent([
					// Mandatory fields
                    'Artikelnummer',
                    'Produkttitel',
                    'Bild-URL',
                    'Deeplink',
                    'Produkt-Kategorie',
                    'Produkt-Beschreibung',
                    'Preis',
                    'Währung',
                    'Marke',
                    'Versandkosten',
                    'Geschlecht', // only mandatory for chlotes
                    'Grundpreis', // only mandatory for cosmetics
                    // Optional fields
                    'Streichpreis',
                    'Lieferzeit',
                    'Produktstamm-ID',
                    'EAN',
                    'Bild2-URL',
                    'Bild3-URL',
                    'Bild4-URL',
                    'Bild5-URL',
                    'Größe',
                    'Farbe',
                    'Material',
			    ]);

			foreach($resultData as $item)
			{
                $rrp = $item->variationRecommendedRetailPrice->price > $this->elasticExportHelper->getPrice($item, $settings) ? $item->variationRecommendedRetailPrice->price : '';

				$data = [

                    // Mandatory fields
                    'Artikelnummer'         => $item->itemBase->id,
                    'Produkttitel'          => $this->elasticExportHelper->getName($item, $settings),
                    'Bild-URL'              => $this->elasticExportHelper->getMainImage($item, $settings),
                    'Deeplink'              => $this->elasticExportHelper->getUrl($item, $settings, true, false),
                    'Produkt-Kategorie'     => $this->elasticExportHelper->getCategory($item->variationStandardCategory->categoryId, $settings->get('lang'), $settings->get('plentyId')),
                    'Produkt-Beschreibung'  => $this->elasticExportHelper->getDescription($item, $settings, 256),
                    'Preis'                 => number_format($this->elasticExportHelper->getPrice($item, $settings), 2, '.', ''),
                    'Währung'               => $item->variationRetailPrice->currency,
                    'Marke'                 => $item->itemBase->producer,
                    'Versandkosten'         => number_format($this->elasticExportHelper->getShippingCost($item, $settings), 2, ',', ''),
                    'Geschlecht'            => $this->getProperty($item, $settings, 'size')?$this->getProperty($item, $settings, 'size'):$this->getStandardGender($settings->get('gender')), // only mandatory for chlotes
                    'Grundpreis'            => $this->elasticExportHelper->getBasePrice($item, $settings), // only mandatory for cosmetics
                    // Optional fields
                    'Streichpreis'          => $rrp,
                    'Lieferzeit'            => $this->elasticExportHelper->getAvailability($item, $settings),
                    'Produktstamm-ID'       => $item->itemBase->id,
                    'EAN'                   => $this->elasticExportHelper->getBarcodeByType($item, $settings, ElasticExportHelper::BARCODE_EAN),
                    'Bild2-URL'             => $this->getImageByNumber($item, $settings, 2),
                    'Bild3-URL'             => $this->getImageByNumber($item, $settings, 3),
                    'Bild4-URL'             => $this->getImageByNumber($item, $settings, 4),
                    'Bild5-URL'             => $this->getImageByNumber($item, $settings, 5),
                    'Größe'                 => $this->getProperty($item, $settings, 'size'),
                    'Farbe'                 => $this->getProperty($item, $settings, 'color'),
                    'Material'              => $this->getProperty($item, $settings, 'material')
				];

				$this->addCSVContent(array_values($data));
			}
        }
    }

    /**
     * Check if gender string is valid.
     * @param string $gender
     * @return string
     */
    private function getStandardGender(string $gender = ''):string
    {
        $standardGender = '';

        $genderList = [ 'male',
                        'female',
                        'unisex'];

        if (strlen($gender) > 0)
        {
            $gender = trim(strtolower($gender));

            if (array_key_exists($gender, $genderList))
            {
                $standardGender = $genderList[$gender];
            }
        }
        return $standardGender;
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
                if($attributeValueLang->attributeValue->tracdelight_map)
                {
                    $variationAttributes[$attributeValueLang->attributeValue->tracdelight_map][] = $attributeValueLang->name;
                }
            }
        }

        $tracdelightAttributes = [];

        foreach($variationAttributes as $attribute)
        {
            if(array_key_exists('Color', $attribute) && is_array($attribute['Color']))
            {
                $tracdelightAttributes['color'] = implode(', ', $attribute['Color']);
            }

            if(array_key_exists('Size', $attribute) && is_array($attribute['Size']))
            {
                $tracdelightAttributes['size'] = implode(', ', $attribute['Size']);
            }

        }

        return $tracdelightAttributes;
    }

    /**
     * Get property.
     * @param  Record   $item
     * @param  KeyValue $settings
     * @param  string   $property
     * @return string
     */
	private function getProperty(Record $item, KeyValue $settings, string $property):string
    {
        if (count($this->itemPropertyCache[$item->itemBase->id]) == 0)
        {
            $this->itemPropertyCache[$item->itemBase->id] =  array_merge($this->getVariationAttributes($item, $settings),$this->getItemPropertyList($item, $settings));
        }

        if(array_key_exists($property, $this->itemPropertyCache[$item->itemBase->id]))
        {
            return $this->itemPropertyCache[$item->itemBase->id][$property];
        }

        return '';
    }


    /**
     * Get item properties.
     * @param 	Record $item
     * @param  KeyValue $settings
     * @return array<string,string>
     */
	protected function getItemPropertyList(Record $item, KeyValue $settings):array<string,string>
	{
        $list = [];

        $characterMarketComponentList = $this->elasticExportHelper->getItemCharactersByComponent($item, $settings);

        if(count($characterMarketComponentList))
        {
            foreach($characterMarketComponentList as $data)
            {
                if( (string) $data['characterValueType'] != 'file' &&
                    (string) $data['characterValueType'] != 'empty' &&
                    (string) $data['externalComponent']  != "0")
                {
                    if((string) $data['characterValueType'] == 'selection')
                    {
                        $characterSelection = $this->characterSelectionRepository->findCharacterSelection((int) $data['characterValue']);
                        if($characterSelection instanceof CharacterSelection)
                        {
                            $list[(string) $data['externalComponent']] = (string) $characterSelection->name;
                        }
                    }
                    else
                    {
                        $list[(string) $data['externalComponent']] = (string) $data['characterValue'];
                    }

                }
            }
        }
        //check gender string
        if (strlen($list['gender']) == 0)
        {
            $this->itemPropertyCache[$item->itemBase->id]['gender'] = $this->getStandardGender($settings->get('gender'));
        }
        else
        {
            $this->itemPropertyCache[$item->itemBase->id]['gender'] = $this->getStandardGender($this->itemPropertyCache[$item->itemBase->id]['gender']);
        }

        return $list;
    }
}
