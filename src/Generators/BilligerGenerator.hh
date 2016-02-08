<?hh // strict
namespace ElasticExport\Generators;

use Plenty\Modules\DataExchange\Contracts\CSVGenerator;
use Plenty\Modules\Helper\Services\ArrayHelper;
use Plenty\Modules\Item\DataLayer\Models\Record;
use Plenty\Modules\Item\DataLayer\Models\RecordList;
use Plenty\Modules\DataExchange\Models\FormatSetting;
use ElasticExport\Helper\ElasticExportHelper;

class BilligerGenerator extends CSVGenerator
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
     * BilligerGenerator constructor.
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
					'aid',
					'name',
					'price',
					'link',
					'brand',
					//'ean',
					'desc',
					'shop_cat',
					//'image',
					'dlv_time',
					//'dlv_cost',
					//'ppu',
					//'mpnr',

			]);

			foreach($resultData as $item)
			{
				$data = [];

				$data['aid'] = $item->itemBase->id;
				$data['name'] = $this->elasticExportHelper->getName($item, $settings);
				$data['price'] = number_format($item->variationRetailPrice->price, 2, '.', '');
				$data['link'] = $this->elasticExportHelper->getUrl($item, $settings, 'http://master.plentymarkets.com', true, false);
				$data['brand'] = $item->itemBase->producer;
				// ean
				$data['desc'] = $this->elasticExportHelper->getDescription($item, $settings, 256);
                $data['shop_cat'] = $this->elasticExportHelper->getCategory($item, $settings);
                $data['dlv_time'] = $this->elasticExportHelper->getAvailability($item, $settings);
                // dlv_cost
                // ppu
                // mpnr

				$this->addCSVContent(array_values($data));
			}
		}
	}
}
