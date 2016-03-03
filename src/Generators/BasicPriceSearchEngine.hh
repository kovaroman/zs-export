<?hh // strict
namespace ElasticExport\Generators;

use Plenty\Modules\DataExchange\Contracts\CSVGenerator;
use Plenty\Modules\Helper\Services\ArrayHelper;
use Plenty\Modules\Item\DataLayer\Models\Record;
use Plenty\Modules\Item\DataLayer\Models\RecordList;
use Plenty\Modules\DataExchange\Models\FormatSetting;
use ElasticExport\Helper\ElasticExportHelper;


class BasicPriceSearchEngine extends CSVGenerator
{
    const string DELIMITER = ' ';
    /*
     * @var ElasticExportHelper
     */
    private ElasticExportHelper $elasticExportHelper;

    /*
     * @var ArrayHelper
     */
    private ArrayHelper $arrayHelper;

    /**
     * BasicPriceSearchEngine constructor.
     * @param ElasticExportHelper $elasticExportHelper
     * @param ArrayHelper $arrayHelper
     */
    public function __construct(ElasticExportHelper $elasticExportHelper, ArrayHelper $arrayHelper)
    {
        $this->elasticExportHelper = $elasticExportHelper;
        $this->arrayHelper = $arrayHelper;
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
                    'unit',
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
                    'shipment_and_handling',
                    'unit_price',
                    'unit_price_value',
                    'unit_price_lot',
			    ]);

			foreach($resultData as $item)
			{
                $rrp = $item->variationRecommendedRetailPrice->price > $this->elasticExportHelper->getPrice($item, $settings) ? $item->variationRecommendedRetailPrice->price : '';

				$data = [
					'article_id'            => $item->itemBase->id,
                    'deeplink'              => $this->elasticExportHelper->getUrl($item, $settings, true, false),
                    'name'                  => $this->elasticExportHelper->getName($item, $settings),
                    'short_description'     => $item->itemDescription->shortDescription,
                    'description'           => $this->elasticExportHelper->getDescription($item, $settings, 256),
                    'article_no'            => $item->variationBase->customNumber,
                    'producer'              => $item->itemBase->producer,
                    'model'                 => $item->variationBase->model,
                    'availability'          => $this->elasticExportHelper->getAvailability($item, $settings),
                    'ean'                   => $this->elasticExportHelper->getBarcodeByType($item, $settings, ElasticExportHelper::BARCODE_EAN),
                    'isbn'                  => $this->elasticExportHelper->getBarcodeByType($item, $settings, ElasticExportHelper::BARCODE_ISBN),
                    'fedas'                 => $item->itemBase->fedas,
                    'unit'                  => '',
                    'price'                 => number_format($this->elasticExportHelper->getPrice($item, $settings), 2, '.', ''),
                    'price_old'             => $rrp,
                    'weight'                => $item->variationBase->weightG,
                    'category1'             => $this->elasticExportHelper->getCategoryBranch($item, $settings, 1),
                    'category2'             => $this->elasticExportHelper->getCategoryBranch($item, $settings, 2),
                    'category3'             => $this->elasticExportHelper->getCategoryBranch($item, $settings, 3),
                    'category4'             => $this->elasticExportHelper->getCategoryBranch($item, $settings, 4),
                    'category5'             => $this->elasticExportHelper->getCategoryBranch($item, $settings, 5),
                    'category6'             => $this->elasticExportHelper->getCategoryBranch($item, $settings, 6),
                    'category_concat'       => $this->elasticExportHelper->getCategory($item->variationStandardCategory->categoryId, $settings->get('lang'), $settings->get('plentyId')),
                    'image_url_preview'     => $this->elasticExportHelper->getImageList($item, $settings, ';', 'preview'),
                    'image_url'             => $this->elasticExportHelper->getMainImage($item, $settings),
                    'shipment_and_handling' => number_format($this->elasticExportHelper->getShippingCost($item, $settings), 2, ',', ''),
                    'unit_price'            => $this->elasticExportHelper->getBasePrice($item, $settings),
                    'unit_price_value'      => '',
                    'unit_price_lot'        => ''
				];

				$this->addCSVContent(array_values($data));
			}
        }
    }
}
