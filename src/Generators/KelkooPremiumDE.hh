<?hh // strict
namespace ElasticExport\Generators;

use Plenty\Modules\DataExchange\Contracts\CSVGenerator;
use Plenty\Modules\Helper\Services\ArrayHelper;
use Plenty\Modules\Item\DataLayer\Models\Record;
use Plenty\Modules\Item\DataLayer\Models\RecordList;
use Plenty\Modules\DataExchange\Models\FormatSetting;
use ElasticExport\Helper\ElasticExportHelper;


class KelkooPremiumDE extends CSVGenerator
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
     * KelkooPremiumDE constructor.
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

			$this->setDelimiter(" ");

			$this->addCSVContent([
					'category',
                    'marke',
                    'title',
                    'description',
                    'price',
                    'deliverycost',
                    'url',
                    'image',
                    'availability',
                    'offerid',
                    'unitaryPrice',
                    'ean',

			]);

			foreach($resultData as $item)
			{
			    $deliveryCost = $this->elasticExportHelper->getShippingCost($item, $settings);
			    if(!is_null($deliveryCost))
                {
                    $deliveryCost = number_format($deliveryCost, 2, ',', '');
                }
                else
                {
                    $deliveryCost = '';
                }

				$data = [
                    'category'      => $this->elasticExportHelper->getCategory($item->variationStandardCategory->categoryId, $settings->get('lang'), $settings->get('plentyId')),
                    'marke'         => $item->itemBase->producer,
                    'title' 		=> $this->elasticExportHelper->getName($item, $settings),
                    'description'   => $this->elasticExportHelper->getDescription($item, $settings, 256),
                    'price' 	    => number_format($this->elasticExportHelper->getPrice($item), 2, '.', ''),
                    'deliverycost' 	=> $deliveryCost,
                    'url' 		    => $this->elasticExportHelper->getUrl($item, $settings, true, false),
                    'image'		    => $this->elasticExportHelper->getMainImage($item, $settings),
                    'availability'  => $this->elasticExportHelper->getAvailability($item, $settings),
                    'offerid'       => $item->variationBase->id,
                    'unitaryPrice'  => $this->elasticExportHelper->getBasePrice($item, $settings),
                    'ean'           => $item->variationBarcode->code,
				];

				$this->addCSVContent(array_values($data));
			}
        }
    }
}
