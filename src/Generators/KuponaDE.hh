<?hh // strict
namespace ElasticExport\Generators;

use Plenty\Modules\DataExchange\Contracts\CSVGenerator;
use Plenty\Modules\Helper\Services\ArrayHelper;
use Plenty\Modules\Item\DataLayer\Models\Record;
use Plenty\Modules\Item\DataLayer\Models\RecordList;
use Plenty\Modules\DataExchange\Models\FormatSetting;
use ElasticExport\Helper\ElasticExportHelper;


class KuponaDE extends CSVGenerator
{
    /*
     * @var ElasticExportHelper
     */
private ElasticExportHelper $elasticExportHelper;

    /*
     * @var ArrayHelper
     */
private ArrayHelper $arrayHelper;

    /**
     * KuponaDE constructor.
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

			$this->setDelimiter(";");

			$this->addCSVContent([
					'prod_number',
                    'prod_name',
                    'prod_price',
                    'prod_price_old',
                    'currency_symbol',
                    'prod_url',
                    'category',
                    'category_url',
                    'valid_from_date',
                    'valid_to_date',
                    'prod_description',
                    'prod_description_long',
                    'img_small',
                    'img_medium',
                    'img_large',
                    'ean_code',
                    'versandkosten',
                    'lieferzeit',
                    'platform',
                    'grundpreis',

                    ]);

			foreach($resultData as $item)
			{
                $rrp = $item->variationRecommendedRetailPrice->price > $this->elasticExportHelper->getPrice($item, $settings) ? $item->variationRecommendedRetailPrice->price : '';

				$data = [
                    'prod_number'           => $item->itemBase->id,
                    'prod_name'             => $this->elasticExportHelper->getName($item, $settings),
                    'prod_price'            => number_format($this->elasticExportHelper->getPrice($item, $settings), 2, '.', ''),
                    'prod_price_old'        => $rrp,
                    'currency_symbol'       => $item->variationRetailPrice->currency,
                    'prod_url'              => $this->elasticExportHelper->getUrl($item, $settings, true, false),
                    'category'              => $this->elasticExportHelper->getCategory($item->variationStandardCategory->categoryId, $settings->get('lang'), $settings->get('plentyId')),
                    'category_url'          => '',
                    'valid_from_date'       => '',
                    'valid_to_date'         => '',
                    'prod_description'      => $this->elasticExportHelper->getPreviewText($item, $settings, 256),
                    'prod_description_long' => $this->elasticExportHelper->getDescription($item, $settings, 256),
                    'img_small'             => $this->elasticExportHelper->getImageList($item, $settings, ';', 'preview'),
                    'img_medium'            => $this->elasticExportHelper->getImageList($item, $settings, ';', 'middle'),
                    'img_large'             => $this->elasticExportHelper->getImageList($item, $settings, ';', 'normal'),
                    'ean_code'              => $this->elasticExportHelper->getBarcodeByType($item, $settings, ElasticExportHelper::BARCODE_EAN),
                    'versandkosten'         => number_format($this->elasticExportHelper->getShippingCost($item, $settings), 2, ',', ''),
                    'lieferzeit'            => $this->elasticExportHelper->getAvailability($item, $settings),
                    'platform'              => '',
                    'grundpreis'            => $this->elasticExportHelper->getBasePrice($item, $settings),

				];

				$this->addCSVContent(array_values($data));
			}
        }
    }
}
