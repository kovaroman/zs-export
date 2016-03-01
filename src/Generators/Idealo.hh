<?hh // strict
namespace ElasticExport\Generators;

use Plenty\Modules\DataExchange\Contracts\CSVGenerator;
use Plenty\Modules\Helper\Services\ArrayHelper;
use Plenty\Modules\Item\DataLayer\Models\Record;
use Plenty\Modules\Item\DataLayer\Models\RecordList;
use Plenty\Modules\DataExchange\Models\FormatSetting;
use ElasticExport\Helper\ElasticExportHelper;
use Plenty\Modules\Helper\Models\KeyValue;

/**
 * Class Idealo
 * @package ElasticExport\Generators
 */
class Idealo extends CSVGenerator
{
	/*
     * @var
     */
    private ElasticExportHelper $elasticExportHelper;

	/*
	 * @var ArrayHelper
	 */
	private ArrayHelper $arrayHelper;

    /**
     * IdealoGenerator constructor.
     * @param ElasticExportHelper $elasticExportHelper
     * @param ArrayHelper $arrayHelper
     */
    public function __construct(ElasticExportHelper $elasticExportHelper, ArrayHelper $arrayHelper)
    {
        $this->elasticExportHelper = $elasticExportHelper;
		$this->arrayHelper = $arrayHelper;
    }

    protected function generateContent(mixed $resultData, array<FormatSetting> $formatSettings = []):void
    {
        if($resultData instanceof RecordList)
		{
			$settings = $this->arrayHelper->buildMapFromObjectList($formatSettings, 'key', 'value');

			$this->setDelimiter(" ");

			$this->addCSVContent([
				'article_id',
				'deeplink',
				'name',
				'short_description',
				'description',
				'article_no',
				'producer',
				'model',
				'availability',
				'ean',
				'isbn',
				'fedas',
				'warranty',
				'price',
				'price_old',
				'weight',
				'category1',
				'category2',
				'category3',
				'category4',
				'category5',
				'category6',
				'category_concat',
				'image_url_preview',
				'image_url',
				'base_price',
			]);

			foreach($resultData as $item)
			{
				$price = $this->elasticExportHelper->getPrice($item, $settings) <= 0 ? $item->variationRecommendedRetailPrice->price : $this->elasticExportHelper->getPrice($item, $settings);
				$rrp = $item->variationRecommendedRetailPrice->price <= $price ? 0 : $item->variationRecommendedRetailPrice->price;

				$variationName = $this->elasticExportHelper->getAttributeValueSetShortFrontendName($item, $settings);

				$data = [
					'article_id' 		=> $item->variationBase->id,
					'deeplink' 			=> $this->elasticExportHelper->getUrl($item, $settings, true, false),
					'name' 				=> $this->elasticExportHelper->getName($item, $settings) . (strlen($variationName) ? ' ' . $variationName : ''),
					'short_description' => $this->elasticExportHelper->getPreviewText($item, $settings),
					'description' 		=> $this->elasticExportHelper->getDescription($item, $settings),
					'article_no' 		=> $item->variationBase->customNumber,
					'producer' 			=> $item->itemBase->producer,
					'model' 			=> $item->variationBase->model,
					'availability' 		=> $this->elasticExportHelper->getAvailability($item, $settings),
					'ean'	 			=> $this->elasticExportHelper->getBarcodeByType($item, $settings, ElasticExportHelper::BARCODE_EAN),
					'isbn' 				=> $this->elasticExportHelper->getBarcodeByType($item, $settings, ElasticExportHelper::BARCODE_ISBN),
					'fedas' 			=> $item->itemBase->fedas,
					'warranty' 			=> '',
					'price' 			=> number_format($price, 2, '.', ''),
					'price_old' 		=> number_format($rrp, 2, '.', ''),
					'weight' 			=> $item->variationBase->weightG,
					'category1' 		=> $this->elasticExportHelper->getCategoryBranch($item, $settings, 1),
					'category2' 		=> $this->elasticExportHelper->getCategoryBranch($item, $settings, 2),
					'category3' 		=> $this->elasticExportHelper->getCategoryBranch($item, $settings, 3),
					'category4' 		=> $this->elasticExportHelper->getCategoryBranch($item, $settings, 4),
					'category5' 		=> $this->elasticExportHelper->getCategoryBranch($item, $settings, 5),
					'category6' 		=> $this->elasticExportHelper->getCategoryBranch($item, $settings, 6),
					'category_concat' 	=> $this->elasticExportHelper->getCategory($item, $settings),
					'image_url_preview' => $this->elasticExportHelper->getImageList($item, $settings, ';', 'preview'),
					'image_url' 		=> $this->elasticExportHelper->getImageList($item, $settings, ';', 'normal'),
					'base_price' 		=> $this->elasticExportHelper->getBasePrice($item, $settings),
					'free_text_field'   => $this->getFreeText($item, $settings),
				];

				$this->addCSVContent(array_values($data));
			}
		}
    }

	/**
	 * Get free text.
	 * @param  Record   $item
	 * @param  KeyValue $settings
	 * @return {string
	 */
	private function getFreeText(Record $item, KeyValue $settings):string
	{

		$characterMarketComponentList = $this->elasticExportHelper->getItemCharactersByComponent($item, $settings, 1);

		$freeText = [];

		if(count($characterMarketComponentList))
		{
			foreach($characterMarketComponentList as $data)
			{
				if((string) $data['characterValueType'] != 'file' && (string) $data['characterValueType'] != 'empty')
				{
					$freeText[] = (string) $data['characterValue'];
				}
			}
		}

		return implode(' ', $freeText);
	}    
}
