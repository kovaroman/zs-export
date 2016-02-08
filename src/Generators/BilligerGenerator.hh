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

	/**
     * BilligerGenerator constructor.
     * @param ItemExportHelper $itemExportHelper
     */
    public function __construct(ItemExportHelper $itemExportHelper)
    {
        $this->itemExportHelper = $itemExportHelper;
    }

	/**
	 * @param mixed $resultData
	 */
	protected function generateContent(mixed $resultData, array<FormatSetting> $formatSettings = []):void
	{

		if($resultData instanceof RecordList)
		{
			$settings = array();

			foreach($formatSettings as $formatSetting)
			{
				$settings[$formatSetting->key] = $formatSetting->value;
			}
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
				$data['name'] = $this->itemExportHelper->getName($item, $settings);
				$data['price'] = number_format($item->variationRetailPrice->price, 2, '.', '');
				// link
				$data['brand'] = $item->itemBase->producer;
				// ean
				$data['desc'] = $this->itemExportHelper->getDescription($item, $settings, 256);
                // shop_cat
                //
                // §i

                $data['dlv_time'] = $this->getAvailability($item, 0 , 50);
                // dlv_cost
                // ppu
                // mpnr


				$this->addCSVContent(array_values($data));
			}
		}
	}

	private function getAvailability(Record $item, int $defaultReturnValue = 0, int $returnAverageDays = 0):string
	{
		return 'available';
	}
}
