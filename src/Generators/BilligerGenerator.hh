<?hh // strict
namespace ElasticExport\Generators;

use Plenty\Modules\DataExchange\Contracts\CSVGenerator;
use Plenty\Modules\Item\DataLayer\Models\Record;
use Plenty\Modules\Item\DataLayer\Models\RecordList;
use Plenty\Modules\DataExchange\Models\FormatSetting;
use ElasticExport\Helper\ItemExportHelper;

class BilligerGenerator extends CSVGenerator
{
	/*
     * @var ItemExportHelper
     */
    private ItemExportHelper $itemExportHelper;

	/*
	 * @var ArrayHelper
	 */
	private ArrayHelper $arrayHelper;

	/**
     * BilligerGenerator constructor.
     * @param ItemExportHelper $itemExportHelper
     * @param ArrayHelper $arrayHelper
     */
    public function __construct(ItemExportHelper $itemExportHelper, ArrayHelper $arrayHelper)
    {
        $this->itemExportHelper = $itemExportHelper;
		$this->arrayHelper = $arrayHelper;
    }

	/**
	 * @param mixed $resultData
	 */
	protected function generateContent(mixed $resultData, array<FormatSetting> $formatSettings = []):void
	{
		if($resultData instanceof RecordList)
		{
			$settingsMap = $this->arrayHelper->buildMapFromObjectList($formatSettings, 'key', 'value');

			$this->setDelimiter(";");

			$this->addCSVContent([
					'aid',
					'name',
					'price',
					//'link',
					'brand',
					//'ean',
					'desc',
					//'shop_cat',
					//'image',
					'dlv_time',
					//'dlv_cost',
					//'ppu',
					//'mpnr',

			]);

			foreach($resultData as $item)
			{
				$data = array();

				$data['aid'] = $item->itemBase->id;
				$data['name'] = $this->itemExportHelper->getName($item, $settingsMap);
				$data['price'] = number_format($item->variationRetailPrice->price, 2, '.', '');
				// link
				$data['brand'] = $item->itemBase->producer;
				// ean
				$data['desc'] = $this->itemExportHelper->getDescription($item, $settingsMap, 256);
                // shop_cat
                //
                // §i

                $data['dlv_time'] = $this->getAvailability($item, $settingsMap);
                // dlv_cost
                // ppu
                // mpnr


				$this->addCSVContent(array_values($data));
			}
		}
	}

	private function getAvailability(Item $item, array<string, string>$settingsMap):int
	{		

		return 0;
	}

}
