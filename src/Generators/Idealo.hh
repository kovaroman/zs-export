<?hh // strict
namespace ElasticExport\Generators;

use Plenty\Modules\DataExchange\Contracts\CSVGenerator;
use Plenty\Modules\Helper\Services\ArrayHelper;
use Plenty\Modules\Item\DataLayer\Models\Record;
use Plenty\Modules\Item\DataLayer\Models\RecordList;
use Plenty\Modules\DataExchange\Models\FormatSetting;
use ElasticExport\Helper\ElasticExportHelper;

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
				$data = [
					'article_id' 		=> $item->variationBase->id,
					'deeplink' 			=> $this->elasticExportHelper->getUrl($item, $settings, true, false),
					'name' 				=> $this->elasticExportHelper->getName($item, $settings),
					'short_description' => $this->elasticExportHelper->getPreviewText($item, $settings),
					'description' 		=> $this->elasticExportHelper->getDescription($item, $settings),
					'article_no' 		=> $item->variationBase->customNumber,
					'producer' 			=> $item->itemBase->producer,
					'model' 			=> $item->variationBase->model,
					'availability' 		=> $this->elasticExportHelper->getAvailability($item, $settings),
					'ean'	 			=> $this->elasticExportHelper->getBarcodeByType($item, $settings, ElasticExportHelper::BARCODE_EAN),
					'isbn' 				=> $this->elasticExportHelper->getBarcodeByType($item, $settings, ElasticExportHelper::BARCODE_ISBN),
					'fedas' 			=> $item->itemBase->fedas,
					'warranty' 			=> '', // TODO warranty
					'price' 			=> number_format($this->elasticExportHelper->getPrice($item, $settings), 2, '.', ''),
					'price_old' 		=> '', // TODO get UVP price,
					'weight' 			=> $item->variationBase->weightG,
					'category1' 		=> '', // TODO category1
					'category2' 		=> '', // TODO category2
					'category3' 		=> '', // TODO category3
					'category4' 		=> '', // TODO category4
					'category5' 		=> '', // TODO category5
					'category6' 		=> '', // TODO category6
					'category_concat' => $this->elasticExportHelper->getCategory($item, $settings),
					'image_url_preview' => $this->elasticExportHelper->getImages($item, $settings, ';', 'preview'),
					'image_url' => $this->elasticExportHelper->getImages($item, $settings, ';'),
					'base_price' => $this->elasticExportHelper->getBasePrice($item, $settings),
					// TODO free_text_field?
				];

				$this->addCSVContent(array_values($data));
			}
		}
    }
}
